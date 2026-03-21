/*
 * File:   functions.c
 * Author: gregd
 *
 * Created on March 20, 2026, 4:30 PM
 */


#include "xc.h"
#include "functions4.h"
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
    else if(timer == TIMER5){
        if(IFS1bits.T5IF == 1){
            temp = 1;
        }
        while(1){
            if(IFS1bits.T5IF == 1){
                IFS1bits.T5IF = 0;
                if(LD2_flag == 1){
                    // checks if blinking has been stopped (received LD2 odd times)
                   LATGbits.LATG9 = !LATGbits.LATG9; // toggle LD2
                }
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

// Function to setup a 32-bits timer (timer4 and timer5)
void tmr_setup_period_alternative(int ms){
    uint32_t Fcy = 72000000; // dsPIC33EP512MU810
    uint32_t cycles = Fcy / 1000;
    cycles *= ms; // avoids overflow with Fcy*ms/1000
    T4CONbits.TON = 0; // ensure timer is off
    T5CONbits.TON = 0;
    T4CONbits.T32 = 1; // enabling 32-bit timer
    T4CONbits.TCS = 0;
    T4CONbits.TGATE = 0;
    T4CONbits.TCKPS = 0; // 1:1 prescaler otherwise overflow
    TMR4 = 0; // reset timer 4
    TMR5 = 0; // reset timer 5
    
    PR4 = (uint16_t)(cycles & 65535); // lsw (taking least significant 16 bits)
    PR5 = (uint16_t)(cycles >> 16); // msw (taking most significant 16 bits)
    
    IFS1bits.T4IF = 0; // clear interrupt flag
    IFS1bits.T5IF = 0; // clear interrupt flag
    // no interrupt (busy waiting)
    //IEC1bits.T5IE = 1; // (enable interrupt)
    //IPC7bits.T5IP = 0x01; // set priority level for timer 5
    
    T4CONbits.TON = 1;
    return;
}

// Redefinition of ISR for UART1 Rx
// Assignment 1 version
/*
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0;
    char_1 = U1RXREG;
    U1TXREG = char_1;
    LATAbits.LATA0 = !LATAbits.LATA0; // toggle LD1
}
*/

// Redefinition of ISR for UART1 Rx
// Assignment 2 version
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0;
    char_counter++;
    
    // storing characters
    char_3 = char_2;
    char_2 = char_1;
    char_1 = U1RXREG;
    if(char_3 == 'L'){
        if(char_2 == 'D'){
            if(char_1 == '1'){
                LATAbits.LATA0 = !LATAbits.LATA0; // toggle LD1
            }
            else if(char_1 == '2'){
                LD2_flag = !LD2_flag; // stop/resume LD2 blinking
            }
        }
    }
}

// Redefinition of ISR for external interrupt 1
void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt(void){
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt
    send_T2_msg = 1;
}

// Redefinition of ISR for external interrupt 2
void __attribute__((__interrupt__, __auto_psv__)) _INT2Interrupt(void){
    IFS1bits.INT2IF = 0; // clearing flag of external interrupt
    send_T3_msg = 1;
}

// Redefinition of ISR for timer 5
/*
blinking of LD2 with interrupts
void __attribute__((__interrupt__, __auto_psv__)) _T5Interrupt(void){
    IFS1bits.T5IF = 0; // clearing timer flag
    if(LD2_flag == 1){
        // checks if blinking has been stopped (received LD2 odd times)
        LATGbits.LATG9 = !LATGbits.LATG9; // toggle LD2
    }
}
*/