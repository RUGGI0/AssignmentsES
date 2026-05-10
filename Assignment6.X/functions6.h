#ifndef FUNCTIONS6_H
#define	FUNCTIONS6_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define TIMER1 1 // for part 1 to wait ending of sampling time

// for part 3 to recover value at 1kHz and send value at 10Hz to UART
#define TIMER2 2 
#define TIMER3 3

#define SIZETX 64

typedef struct {
    char* buffer; 
    int size;
    volatile int head;
    volatile int tail;
} CircularBuffer;

extern char tx_array[SIZETX];
extern volatile CircularBuffer tx_buffer;
extern volatile int AN5_value;
extern volatile int AN11_value;
extern volatile int analog_pins_flag;
extern volatile int send_to_uart_flag;

void part_1_assignment();
void part_2_assignment();
void part_3_assignment();

void UART_configuration();

// Function to setup timer counting up to 200ms (max value)
void tmr_setup_period(int timer, int ms);
// Function to setup and expire a timer with provided ms (max 200)
void tmr_wait_ms(int timer, int ms);

// Functions to manage Tx and Rx buffers
void buffer_init(volatile CircularBuffer* cb, char* array_ptr, int max_size);
int buffer_is_empty(volatile CircularBuffer* cb);
int buffer_read(volatile CircularBuffer* cb, char* c);
int buffer_write(volatile CircularBuffer* cb, char c);

void send_battery_to_uart(long Vbattery_mV);
void send_sensor_distance_to_uart(double distance);
void send_values_to_uart(double Vbattery, double distance_cm);

#endif