#ifndef PROJECT_FUNCTIONS_H
#define	PROJECT_FUNCTIONS_H

#include <xc.h> 


// --- Timers --- //
#define TIMER1 1 // main loop heartbeat
#define TIMER3 3 // button E8 press (robust implementation)
#define TIMER4 4 // button E9 press (robust implementation)

// --- Tx and Rx buffers --- //
#define SIZERX 16 // buffer emptied every 15ms -> baud rate = 9600 -> at maximum 14 char received
#define SIZETX 64 // da rivedere

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
	char msg_type[7]; // message type is up to 6 chars + string terminator
	char msg_payload[11];  // assume payload cannot be longer than 100 chars
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
#define AVOIDANCE_STEP_1 (3)
#define AVOIDANCE_STEP_2 (4)
#define AVOIDANCE_STEP_3 (5)

// parameter for tasks structure
typedef struct{
    int speed;
    int yaw;
    int robot_state;
    int robot_sub_state;
    int distance_sensor_value; // centimeters
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
void tmr_setup_period(int timer, int ms);
int tmr_wait_period(int timer);
void scheduler(heartbeat schedInfo[], int nTasks);
int parse_byte(parser_state* ps, char byte);
int next_value(const char* msg, int i);
int extract_integer(const char* str);
void task_PWM_set(void* param);
void PWM_set(int speed, int yaw);
void DC_assigning(int RD1, int RD2, int RD3, int RD4);
void task_button_check(void* param);
void task_processing_IR_value_n_sending_to_uart(void* param);
void task_processing_VBAT_value_n_sending_to_uart();


#endif

