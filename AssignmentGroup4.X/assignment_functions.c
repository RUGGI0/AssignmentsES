/*
 * File:   assignment_functions.c
 * Author: gregd
 *
 * Created on April 27, 2026, 1:59 PM
 */


#include "xc.h"
#include <stdio.h>
#include "assignment_functions.h"

char rx_array[SIZERX];
char tx_array[SIZETX];
volatile CircularBuffer rx_buffer;
volatile CircularBuffer tx_buffer;

void algorithm(){
    tmr_wait_ms(TIMER2, 7);
}

void tmr_setup_period(int timer, int ms){
    int cycle_case = -1;
    long Fcy = 72000000; // dsPIC33EP512MU810
    long cycles = Fcy / 1000;
    cycles *= ms; // avoids overflow with Fcy*ms/1000
    if(cycles <= 65535){
        cycle_case = 0;
    }
    else if(cycles/8 <= 65535){
        cycle_case = 1;
        cycles /= 8;
    }
    else if(cycles/64 <= 65535){
        cycle_case = 2;
        cycles /= 64;
    }
    else{
        cycle_case = 3;
        cycles /= 256;
    }
    if(timer == TIMER1){
        T1CONbits.TCS = 0;
        T1CONbits.TGATE = 0;
        T1CONbits.TON = 0; // ensure timer is off
        TMR1 = 0; // reset timer 1
        IFS0bits.T1IF = 0; // clear state period flag before restarting
        if(cycle_case == 0){
            T1CONbits.TCKPS = 0b00; // 1:1 prescaler
        }
        else if(cycle_case == 1){
            T1CONbits.TCKPS = 0b01; // 1:8 prescaler
        }
        else if(cycle_case == 2){
            T1CONbits.TCKPS = 0b10; // 1:64 prescaler
        }
        else{
            T1CONbits.TCKPS = 0b11; // 1:256 prescaler
        }
        PR1 = cycles;
        T1CONbits.TON = 1;
    }
    if(timer == TIMER2){
        T2CONbits.TCS = 0;
        T2CONbits.TGATE = 0;
        T2CONbits.TON = 0; // ensure timer is off
        TMR2 = 0; // reset timer 1
        IFS0bits.T2IF = 0; // clear state period flag before restarting
        if(cycle_case == 0){
            T2CONbits.TCKPS = 0b00; // 1:1 prescaler
        }
        else if(cycle_case == 1){
            T2CONbits.TCKPS = 0b01; // 1:8 prescaler
        }
        else if(cycle_case == 2){
            T2CONbits.TCKPS = 0b10; // 1:64 prescaler
        }
        else{
            T2CONbits.TCKPS = 0b11; // 1:256 prescaler
        }
        PR2 = cycles;
        T2CONbits.TON = 1;
    }
}

void tmr_wait_ms(int timer, int ms){
    tmr_setup_period(timer, ms);
    if (timer == TIMER1) {
        while(IFS0bits.T1IF == 0);
        T1CONbits.TON = 0;
        IFS0bits.T1IF = 0;
    } else if (timer == TIMER2) {
        while(IFS0bits.T2IF == 0);
        T2CONbits.TON = 0;
        IFS0bits.T2IF = 0;
    }
}

int tmr_wait_period(int timer){
    int temp = 0;
    if(timer == TIMER1){
        if(IFS0bits.T1IF == 1){
            temp = 1;
        }
        while(1){
            if(IFS0bits.T1IF == 1){
                // flag set
                IFS0bits.T1IF = 0; // flag cleared
                break;
            }
        }
    }
    return temp;
}

void buffer_init(volatile CircularBuffer* cb, char* array_ptr, int max_size) {
    cb->buffer = array_ptr; 
    cb->size = max_size;
    cb->head = 0;
    cb->tail = 0;
}


int buffer_is_empty(volatile CircularBuffer* cb) {
    return cb->head == cb->tail;
}

int buffer_write(volatile CircularBuffer* cb, char c) {
    int next = (cb->head + 1) % cb->size;
    if (next == cb->tail) {
        return -1; // Buffer is full
    }
    cb->buffer[cb->head] = c;
    cb->head = next;
    return 0;
}

int buffer_read(volatile CircularBuffer* cb, char* c) {
    if (buffer_is_empty(cb)) {
        return -1; // Buffer is empty
    }
    *c = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % cb->size;
    return 0;
}

int get_accelerometer_value(unsigned int adr){
    unsigned int output1 = 0;
    unsigned int output2 = 0;
    LATBbits.LATB3 = 0;
    spi_write(adr | 0x80);
    output1 = spi_write(0x00); // 0x02 | 0x04 | 0x06
    output2 = spi_write(0x00); // 0x03 | 0x05 | 0x07
    LATBbits.LATB3 = 1;

    output1 = output1 & 0x00F0; // clear last 4 bits
    output2 = output2 << 8; // left shift of 8 bits
    int data = output2 | output1;
    data = data / 16; // right shift of 16 bits
    return data;
}

void set_accelerometer_bandwidth(unsigned int input){
    LATBbits.LATB3 = 0;
    spi_write(0x10);
    spi_write(input);
    LATBbits.LATB3 = 1;
}

unsigned int spi_write(unsigned int data){
    // writing data provided
    while(SPI1STATbits.SPITBF == 1); // wait until Tx buffer is empty
    SPI1BUF = data; // send data (could be greater than a byte)
    
    // reading answer
    while(SPI1STATbits.SPIRBF == 0); // wait until Rx buffer is full
    data = SPI1BUF;
    return data;
}

void send_error_to_uart(){
    char msg[8] = "$ERR,1*";
    for(int i = 0;i<8;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt -> triggers interrupt
}

void send_accelerometer_values_to_uart(int acc_x, int acc_y, int acc_z){
    char msg[24] = "";
    sprintf(msg,"$ACC,%d,%d,%d*", acc_x, acc_y, acc_z);
    for(int i = 0;i<24;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt
}

void send_roll_pitch_to_uart(int roll, int pitch){
    char msg[16] = "";
    sprintf(msg,"$ANG,%d,%d*", roll, pitch);
    for(int i = 0;i<16;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt
}

void send_miss(int miss_counter){
    char msg[16] = "";
    sprintf(msg,"+++%d++*", miss_counter);
    for(int i = 0;i<16;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt
}

// ISR redefinition for UART1 Rx register
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0;
    
    // Reading all characters in the Rx buffer (until empty) and writing them in the circular buffer
    while(U1STAbits.URXDA == 1){
        buffer_write(&rx_buffer, U1RXREG);
    }
}

// ISR redefinition for UART1 Tx register
void __attribute__((__interrupt__, __auto_psv__)) _U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0;
    
    // Reading all characters in the circular buffer buffer (until empty),
    // and writing them in the Tx buffer until it is full
    while(U1STAbits.UTXBF == 0 && !buffer_is_empty(&tx_buffer)){
        // - checking if there is anything to send, since the interrupt will trigger right after enabling U1TX
        // - Tx buffer is not full, at least one more char can be written
            char c;
            buffer_read(&tx_buffer, &c);
            U1TXREG = c;
    }
    
    if(buffer_is_empty(&tx_buffer)){
        // Tx buffer could be full or not, in any case no more data to send
        IEC0bits.U1TXIE = 0; // disable interrupt to avoid multiple triggers
    }
    
    // In case the circular buffer is not empty, the enabler remains set, 
    // so, as soon as one byte is sent, thus freeing on slot in the Tx buffer,
    // the interrupt will trigger again
}