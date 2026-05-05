/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> 

#define TIMER1 1
#define TIMER2 2

#define SIZETX 64
#define SIZERX 16

// max num caratteri inviati a burst (tutti insime)
// in Tx 42 caratteri, (20 + 15 + 7) -> limitazione su buffer size minima
// in Rx 14 (7+7) caratteri (vincoli sull'utente)

// baud rate minima per worst case scenario 2750 (solo roba periodica)

// prova a diminunire la size dei buffer e vedere se messaggi vengono tagliati

typedef struct {
    char* buffer; 
    int size;
    volatile int head;
    volatile int tail;
} CircularBuffer;

/*
typedef struct {
    char buffer[SIZE2];
    volatile int head;
    volatile int tail;
} CircularBufferRx;
*/

extern char rx_array[SIZERX];
extern char tx_array[SIZETX];
extern volatile CircularBuffer rx_buffer;
extern volatile CircularBuffer tx_buffer;

void algorithm();
void tmr_setup_period(int timer, int ms);
void tmr_wait_ms(int timer, int ms);
int tmr_wait_period(int timer);
void buffer_init(volatile CircularBuffer* cb, char* array_ptr, int max_size);
int buffer_is_empty(volatile CircularBuffer* cb);
int buffer_read(volatile CircularBuffer* cb, char* c);
int buffer_write(volatile CircularBuffer* cb, char c);
unsigned int spi_write(unsigned int data);
int get_accelerometer_value(unsigned int adr);
void send_error_to_uart();
void set_accelerometer_bandwidth(unsigned int input);
void send_accelerometer_values_to_uart(int acc_x, int acc_y, int acc_z);
void send_roll_pitch_to_uart(int roll, int pitch);



#endif	/* XC_HEADER_TEMPLATE_H */

