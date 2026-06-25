#ifndef PROJECT_FUNCTIONS_H
#define	PROJECT_FUNCTIONS_H

#include <xc.h> 


// --- Timers --- //
#define TIMER1 1 // main loop heartbeat
#define TIMER2 2 // timer to wait for IMU board start up
#define TIMER3 3 // button E8 press (robust implementation)
#define TIMER4 4 // button E9 press (robust implementation)

// ---* Baud Rate configuration *---
// lowest baud rate necessary in worst case scenario is 3520 (considering only periodic messages):
// - MBATT message sent at maximum 1 Hz (12 char) -> 12 char/s
// - MDIST message sent at maximum 10 Hz (11 char) -> 110 char/s
// - MANGLE message sent at maximum 10 Hz (23 char) -> 230 char/s
// - MBUF message sent at maximum 10 Hz (11 char) -> 110 char/s
// total = (12 + 110 + 230 + 110) char/s = 462 char/s
// UART protocol uses 10 bit for each char -> 462 * 10 = 4620 bit/s
// We choose a baud rate of 9600 to guarantee that at maximum 1.92 char are received in a main cycle (2 ms) (see line 31)

// --- Tx and Rx buffers --- //
#define SIZETX 64 // Tx buffer size -> maximum number of char in a single burst is 57:
// - highest amount of char composing a MBATT message is 12
// - highest amount of char composing a MDIST message is 11
// - highest amount of char composing a MANGLE message is 23
// - highest amount of char composing a MBUF message (triggered by RE9) is 11 
// this ensures all chars to be sent are saved in the buffer without loss

#define SIZERX 8 // Rx buffer size -> with baud rate = 9600
// the maximum amount of char received in a single control loop cycle (500Hz -> 2 ms) is ~1.92

// #define SIZERX 16 // buffer emptied every 15ms -> baud rate = 9600 -> at maximum 14 char received

typedef struct {
    char* buffer; 
    int size;
    volatile int head;
    volatile int tail;
} CircularBuffer;

extern char rx_array[SIZERX];
extern char tx_array[SIZETX];
extern volatile CircularBuffer rx_buffer;
extern volatile CircularBuffer tx_buffer;

// --- Parser data --- //
#define STATE_DOLLAR  (1) // we discard everything until a dollar is found
#define STATE_TYPE    (2) // we are reading the type of msg until a comma is found
#define STATE_PAYLOAD (3) // we read the payload until an asterix is found
#define NEW_MESSAGE (1) // new message received and parsed completely
#define NO_MESSAGE (0) // no new messages

typedef struct { 
	int state;
	char msg_type[6]; // message type is up to 5 chars + string terminator
	char msg_payload[10];  // assume payload cannot be longer than 10 chars
	int index_type;
	int index_payload;
} parser_state; 

// --- Scheduler data --- //
#define MAX_TASKS 10
// PWM updated (500Hz), IR sensor voltage read (10Hz), battery voltage read (1Hz)
// buggy lights blinking (1Hz for movement or static light), E8 press - E9 press (busy checking or interrupt), 
// accelerometer values - magnetometer values + roll/pitch/yaw computation (can be grouped -> all at 10Hz ),
// TX over UART

typedef struct {
    int n;
    int N;
    int enable;
    void (*f)(void *);
    void* params;
} heartbeat;

// --- Robot states --- //
#define HALTED_STATE (0)
#define MOVING_STATE (1)
#define OBSTACLE_AVOIDANCE_STATE (2)

// Robot sub-states //
#define AVOIDANCE_STEP_0 (3) // obstacle avoidance mode is not active or cannot function (maximum amount has been hit)
#define AVOIDANCE_STEP_1 (4) // turning 90° clockwise
#define AVOIDANCE_STEP_2 (5) // moving forward for 2 seconds
#define AVOIDANCE_STEP_3 (6) // turning 90° anti-clockwise (back to previous heading)
#define AVOIDANCE_STEP_4 (7) // checking if there is still obstacle and if maximum obstacle avoidance executions have been reached

// structure for tasks parameter
typedef struct{
    int speed;
    int yaw;
    int robot_state;
    int robot_sub_state;
    int obs_av_state_ctrl; // counts consequent obstacle avoidance policy
    int distance_sensor_value; // centimetres
    int angle_values[3]; // roll, pitch, yaw (of magnetometer)
    float gyro_yaw;
    float ctrl_yaw; // used to carry out obstacle avoidance policy
    int one_time_exec; // used to ensure that variables like ctrl_yaw, obs_av_state_ctrl are updated only once
    // inside their corresponding case in task_PWM_set(periodic task)
    heartbeat *schedInfo;
    parser_state *par_state;
}control_data;

// Global variable //
extern volatile int AN11_value;
extern volatile int AN14_value;
extern volatile int button_E8_pressed;
extern volatile int button_E9_pressed;

void device_init();

void buffer_init(volatile CircularBuffer* cb, char* array_ptr, int max_size);
int buffer_is_empty(volatile CircularBuffer* cb);
int buffer_write(volatile CircularBuffer* cb, char c);
int buffer_read(volatile CircularBuffer* cb, char* c);
int buffer_occupancy(volatile CircularBuffer* cb);

void tmr_setup_period(int timer, int ms);
int tmr_wait_period(int timer);
void tmr_wait_ms(int timer, int ms);

int parse_byte(parser_state* ps, char byte);
int next_value(const char* msg, int i);
int extract_integer(const char* str);
void PWM_set(int speed, int yaw);
void DC_assigning(int RD1, int RD2, int RD3, int RD4);
unsigned int spi_write(unsigned int data);
int get_accelerometer_value(unsigned int adr);

void task_read_speed_yaw(void* param);
void task_PWM_set(void* param);
void task_stop_buggy_after_2sec (void* param);
void task_button_check(void* param);
void task_reading_VBAT_n_sending_to_uart();
void task_reading_IR_value(void* param);
void task_sending_IR_value_to_uart(void* param);
void task_buggy_lights(void* param);
void task_reading_magn_acc_gyro(void* param);
void sending_angle_values_to_uart(void* param);

void scheduler(heartbeat schedInfo[], int nTasks);

#endif

