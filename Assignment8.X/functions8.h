#ifndef FUNCTIONS8_H
#define	FUNCTIONS8_H

#define TIMER1 1

#define SIZERX 16 // buffer emptied every 15ms -> baud rate = 9600 -> at maximum 14 char received
#define MAX_TASKS 4

typedef struct {
    char* buffer; 
    int size;
    volatile int head;
    volatile int tail;
} CircularBuffer;

extern char rx_array[SIZERX];
extern volatile CircularBuffer rx_buffer;

typedef struct{
    int n;
    int N;
    int enable;
    void (*f)(void *);
    void* params;
} heartbeat;

extern volatile heartbeat schedInfo[MAX_TASKS];

// parameter for tasks structure to avoid using global variables
typedef struct{
    int speed;
    int yaw;
    int new_values; // tells if new valid values has been received
}control_data;

// Functions to manage Rx buffer
void buffer_init(volatile CircularBuffer* cb, char* array_ptr, int max_size);
int buffer_is_empty(volatile CircularBuffer* cb);
int buffer_read(volatile CircularBuffer* cb, char* c);
int buffer_write(volatile CircularBuffer* cb, char c);

void tmr_setup_period(int timer, int ms);
int tmr_wait_period(int timer);
void scheduler();
void task1_update_feedback(void* param);
void task2_update_feedback(void* param);
void task3_update_feedback(void* param);
void task4_update_feedback(void* param);

int parse_value_from_uart(char msg[15], int* tail, int* value);

#endif