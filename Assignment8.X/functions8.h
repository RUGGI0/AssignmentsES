#ifndef FUNCTIONS8_H
#define	FUNCTIONS8_H

#define TIMER1 1

#define SIZETX 64
#define MAX_TASKS 4

typedef struct {
    char* buffer; 
    int size;
    volatile int head;
    volatile int tail;
} CircularBuffer;

extern char rx_array[SIZETX];
extern volatile CircularBuffer rx_buffer;

typedef struct{
    int n;
    int N;
    int enable;
    void (*f)(void *);
    void* params;
} heartbeat;

extern volatile heartbeat schedInfo[MAX_TASKS];

typedef struct{
    // parameters for tasks
}control_data;

extern volatile control_data controlData; // defined in main

void tmr_setup_period(int timer, int ms);
int tmr_wait_period(int timer);
void scheduler();
void task_update_feedback(void* param);


#endif