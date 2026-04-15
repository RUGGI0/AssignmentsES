/*
 * File:   functions.c
 * Author: gregd
 *
 * Created on March 20, 2026, 4:30 PM
 */


#include "xc.h"
#include "functions4.h"

volatile CircularBuffer rx_buffer;
volatile char char_1 = 'a';
volatile char char_2 = 'a';
volatile char char_3 = 'a';
volatile int char_counter = 0;
volatile int LD2_flag = 1;
volatile int send_T2_msg = 0;
volatile int send_T3_msg = 0;

void first_part(){
    // dispic33_datasheet.pdf -> page 9 (microcontroller scheme):
    // RD11 -> RPI75
    // RD0 -> RP64
    // dispic33_datasheet.pdf -> page 211 (RP input):
    // UART1 Rx -> RPINR18bits.U1RXR
    // dispic33_datasheet.pdf -> page 215 (values of functionalities for RP):
    // UART1 Tx -> U1TX: 000001
    // dispic33_datasheet.pdf -> page 98 (RP output):
    // RD0 -> RPOR0bits.RP64R
    // dspic33_rfm_uart-pdf -> page 16: transmitting; page 22: receiving

    TRISDbits.TRISD0 = 0; // Tx in output
    TRISDbits.TRISD11 = 1; // Rx in input
    TRISAbits.TRISA0 = 0; // LD1 in output
    LATAbits.LATA0 = 0; // initial value of LD1

    // Remapping pins
    RPINR18bits.U1RXR = 75; // UART1 Rx mapped to RD11
    RPOR0bits.RP64R = 1; // RD0 (RP64) mapped to UART1 Tx
    INTCON2bits.GIE = 1; // allow enabling interrupts

    // configuring UART1 to have baud rate = 9600 -> [72 000 000/(16*9600)] - 1
    U1BRG = 468; // 467.75
    U1MODEbits.STSEL = 0; // 1 stop bit
    U1MODEbits.PDSEL = 0; // no parity bits - 8 data bits
    U1MODEbits.ABAUD = 0; // no auto baud rate
    U1MODEbits.BRGH = 0; // low-speed mode
    IFS0bits.U1RXIF = 0; // clearing Rx interrupt flag
    IPC2bits.U1RXIP = 1; // priority for Rx
    IPC3bits.U1TXIP = 0; // priority for Tx (interrupt not needed)
    IEC0bits.U1RXIE = 1; // enabling interrupt for UART1 Rx
    U1STAbits.URXISEL = 0; // interrupt if a character is received
    U1MODEbits.UARTEN = 1; // enable UART1
    U1STAbits.UTXEN = 1; // enable U1TX (transmission)

    buffer_init(&rx_buffer);

    while(1){
        if(U1STAbits.OERR == 1){
            // buffer full -> char is read by ISR
            U1STAbits.OERR = 0; // clears buffer
        }
    }
}

// Function to setup timer counting up to 200ms (max value)
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
        return;
    }
    else if(timer == TIMER2){
        T2CONbits.TCS = 0;
        T2CONbits.TGATE = 0;
        T2CONbits.TON = 0; // ensure timer is off
        TMR2 = 0; // reset timer 2
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
        return;
    }
    else if(timer == TIMER3){
        T3CONbits.TCS = 0;
        T3CONbits.TGATE = 0;
        T3CONbits.TON = 0; // ensure timer is off
        TMR3 = 0; // reset timer 2
        IFS0bits.T3IF = 0; // clear state period flag before restarting
        if(cycle_case == 0){
            T3CONbits.TCKPS = 0b00; // 1:1 prescaler
        }
        else if(cycle_case == 1){
            T3CONbits.TCKPS = 0b01; // 1:8 prescaler
        }
        else if(cycle_case == 2){
            T3CONbits.TCKPS = 0b10; // 1:64 prescaler
        }
        else{
            T3CONbits.TCKPS = 0b11; // 1:256 prescaler
        }
        PR3 = cycles;
        T3CONbits.TON = 1;
        return;
    }
    else if(timer == TIMER4){
        T4CONbits.TCS = 0;
        T4CONbits.TGATE = 0;
        T4CONbits.TON = 0; // ensure timer is off
        TMR4 = 0; // reset timer 2
        IFS1bits.T4IF = 0; // clear state period flag before restarting
        if(cycle_case == 0){
            T4CONbits.TCKPS = 0b00; // 1:1 prescaler
        }
        else if(cycle_case == 1){
            T4CONbits.TCKPS = 0b01; // 1:8 prescaler
        }
        else if(cycle_case == 2){
            T4CONbits.TCKPS = 0b10; // 1:64 prescaler
        }
        else{
            T4CONbits.TCKPS = 0b11; // 1:256 prescaler
        }
        PR4 = cycles;
        T4CONbits.TON = 1;
        return;
    }
}

// Variation of void tmr_wait_period (function to clear flag of the expired timer)
// that adds a control variable
int tmr_wait_period_alternative(int timer){
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


// Function to setup and expire a timer with provided ms (max 200)
void tmr_wait_ms(int timer, int ms){
    tmr_setup_period(timer, ms);
    // used only for timer 2
    while(1){
        if(IFS0bits.T2IF == 1){
            T2CONbits.TON = 0;
            IFS0bits.T2IF = 0;
            break;
        }
    }
}

void algorithm(){
    tmr_wait_ms(TIMER2,7);
}

void buffer_init(volatile CircularBuffer* cb) {
    cb->head = 0;
    cb->tail = 0;
}

int buffer_is_empty(volatile CircularBuffer* cb) {
    return cb->head == cb->tail;
}

int buffer_write(volatile CircularBuffer* cb, char c) {
    int next = (cb->head + 1) % SIZE;
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
    cb->tail = (cb->tail + 1) % SIZE;
    return 0;
}

// Redefinition of ISR for UART1 Rx
// Assignment 2 version
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0;
    
    // Reading all characters in the Rx buffer (until empty) and writing them in the circular buffer
    while(U1STAbits.URXDA == 1){
        buffer_write(&rx_buffer, U1RXREG);
    }
    
    /*
    if(U1STAbits.OERR == 1){
        U1STAbits.OERR = 0;
    }
        */
}

// Redefinition of ISR for external interrupt 1 (robust implementation)
void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt(void){
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt
    IPC2bits.T3IP = 0x01; // set priority to one
    IEC0bits.T3IE = 1;
    tmr_setup_period(TIMER3,10);
}

// Redefinition of ISR for external interrupt 2
void __attribute__((__interrupt__, __auto_psv__)) _INT2Interrupt(void){
    IFS1bits.INT2IF = 0; // clearing flag of external interrupt
    IPC6bits.T4IP = 0x01; // set priority to one
    IEC1bits.T4IE = 1;
    tmr_setup_period(TIMER4,10);
}

// Redefinition of ISR for timer 3
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void){
    T3CONbits.TON = 0;
    IFS0bits.T3IF = 0; // clearing timer flag
    IEC0bits.T3IE = 0; // disabling timer interrupt
    
    if(PORTEbits.RE8 == 0){
        send_T2_msg = 1;
    }
}

// Redefinition of ISR for timer 4
void __attribute__((__interrupt__, __auto_psv__)) _T4Interrupt(void){
    T4CONbits.TON = 0;
    IFS1bits.T4IF = 0; // clearing timer flag
    IEC1bits.T4IE = 0; // disabling timer interrupt
    
    if(PORTEbits.RE9 == 0){
        send_T3_msg = 1;
    }
    
}
