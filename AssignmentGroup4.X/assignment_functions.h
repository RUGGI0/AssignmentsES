#ifndef ASSIGNMENTGROUP4_FUNCTIONS_H
#define	ASSIGNMENTGROUP4_FUNCTIONS_H

#include <xc.h> 

#define TIMER1 1
#define TIMER2 2

// ---* Baud Rate configuration *---
// lowest baud rate necessary in worst case scenario is 2750 (considering only periodic messages):
// - ACC message sent at maximum 10 Hz (20 char) -> 200 char/s
// - ANG message sent at maximum 5 Hz (15 char) -> 75 char/s
// total = (200 + 75) char/s = 275 char/s
// UART protocol uses 10 bit for each char -> 275 * 10 = 2750 char/s
// We choose a baud rate of 9600 to guarantee that at least 9 char are received in a main cycle (10 ms) (see line 23)

#define SIZETX 64 // Tx buffer size -> maximum number of char in a single burst is 42:
// - highest amount of char composing a gravitational acceleration message is 20
// - highest amount of char composing a pitch-roll measurement message is 15 
// - highest amount of char composing an error message is 7
// this ensures all chars to be sent are saved in the buffer without loss

#define SIZERX 16 // Rx buffer size -> with baud rate = 9600
// the maximum amount of char received in a main cycle (10 ms) is 9
// (quantity measured in worst case scenario: char delivered at line 144 of main file)

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

void algorithm();
void tmr_setup_period(int timer, int ms);
void tmr_wait_ms(int timer, int ms);
int tmr_wait_period(int timer);

// Functions to manage Tx and Rx buffers
void buffer_init(volatile CircularBuffer* cb, char* array_ptr, int max_size);
int buffer_is_empty(volatile CircularBuffer* cb);
int buffer_read(volatile CircularBuffer* cb, char* c);
int buffer_write(volatile CircularBuffer* cb, char c);

// Function to send new bandwidth to IMU accelerometer through SPI
void set_accelerometer_bandwidth(unsigned int input);
// Function to receive values of IMU accelerometer through SPI
int get_accelerometer_value(unsigned int adr);
// Function to communicate with IMU board through SPI
unsigned int spi_write(unsigned int data);

void send_error_to_uart();
void send_accelerometer_values_to_uart(int acc_x, int acc_y, int acc_z);
void send_roll_pitch_to_uart(int roll, int pitch);

// Debug function to send on UART number of misses on TIMER1
//void send_miss(int miss_counter);

#endif