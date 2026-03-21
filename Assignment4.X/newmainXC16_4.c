/*
 * File:   newmainXC16.c
 * Author: gregd
 *
 * Created on March 20, 2026, 4:28 PM
 */


#include "xc.h"
#include "functions4.h"

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // Assignment 1
    //first_part();
    
    // Assignment 2
    int ret = -1;
    int ret_2 = -1;
    int cycle_counter = 0;
    int mis_counter = 0;
    char T2_msg[6] = "";
    char T3_msg[6] = "";
    TRISAbits.TRISA0 = 0; // LD1 in output
    TRISGbits.TRISG9 = 0; // LD2 in output
    TRISEbits.TRISE8 = 1; // T2 in input
    TRISEbits.TRISE9 = 1; // T3 in input
    LATAbits.LATA0 = 0; // initial value of LD1
    LATGbits.LATG9 = 0; // initial value of LD2
    INTCON2bits.GIE = 1; // allow enabling interrupts
    
    // remap buttons
    RPINR0bits.INT1R = 88; // external interrupt 1 is mapped to button T2 (RP88)
    RPINR1bits.INT2R = 89; // external interrupt 2 is mapped to button T3 (RP89)
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt 1
    IFS1bits.INT2IF = 0; // clearing flag of external interrupt 2
    IPC5bits.INT1IP = 1; // priority level 1
    IPC7bits.INT2IP = 1; // priority level 1
    INTCON2bits.INT1EP = 1; // triggers on falling edge (1->0)
    INTCON2bits.INT2EP = 1; // triggers on falling edge (1->0)
    IEC1bits.INT1IE = 1; // enabling external interrupt 1
    IEC1bits.INT2IE = 1; // enabling external interrupt 2
    
    // set up UART1
    TRISDbits.TRISD0 = 0; // Tx in output
    TRISDbits.TRISD11 = 1; // Rx in input
    RPINR18bits.U1RXR = 75; // UART1 Rx mapped to RD11
    RPOR0bits.RP64R = 1; // RD0 (RP64) mapped to UART1 Tx
    U1BRG = 468; // baud rate = 9600
    U1MODEbits.STSEL = 0; // 1 stop bit
    U1MODEbits.PDSEL = 0; // no parity bits - 8 data bits
    U1MODEbits.ABAUD = 0; // no auto baud rate
    U1MODEbits.BRGH = 0; // low-speed mode
    IFS0bits.U1RXIF = 0; // clearing Rx interrupt flag
    IPC2bits.U1RXIP = 1; // priority for Rx
    IPC3bits.U1TXIP = 0; // priority for Tx (interrupt not needed)
    U1STAbits.URXISEL = 0; // interrupt if a character is received
    IEC0bits.U1RXIE = 1; // enabling interrupt for UART1 Rx
    U1MODEbits.UARTEN = 1; // enable UART1
    U1STAbits.UTXEN = 1; // enable U1TX (transmission)
    
    tmr_setup_period_alternative(400); // managed by interrupt of timer 5
    tmr_setup_period(TIMER1,10);
    while(1){
        
        // executed every 10 ms
        algorithm();
        cycle_counter++;
        
        // -- 10 ms at disposal before other execution of algorithm
        
        if(U1STAbits.OERR == 1){
            // buffer full -> char is read by Rx ISR
            U1STAbits.OERR = 0; // clears buffer
        }
        
        if(send_T2_msg == 1){
            // T2 button has been clicked
            send_T2_msg = 0; // reset flag
            sprintf(T2_msg, "C=%d", char_counter);
            for(int i = 0;i<6;i++){
                if(T2_msg[i] == '\0'){
                    break;
                }
                while(U1STAbits.UTXBF == 1); // Tx buffer is full -> wait
                U1TXREG = T2_msg[i];
            }
        }
        if(send_T3_msg == 1){
            // T3 button has been clicked
            send_T3_msg = 0; // reset flag
            sprintf(T3_msg, "D=%d", mis_counter);
            for(int i = 0;i<6;i++){
                if(T3_msg[i] == '\0'){
                    break;
                }
                while(U1STAbits.UTXBF == 1); // Tx buffer is full -> wait
                U1TXREG = T3_msg[i];
            }
        }
        
        if(cycle_counter == 39){
            cycle_counter = 0;
            ret_2 = tmr_wait_period_alternative(TIMER5); // busy waiting for timer of 400ms
            /*
            if(ret_2){
                LATAbits.LATA0 = 1;
            }
            */
        }
        
        // -- 10 ms shouldn't have already passed, otherwise it arrives late (ret == 1)
        // (it manages to execute code in less than 10ms if timer 5 works with interrupts)
        ret = tmr_wait_period_alternative(TIMER1);
        if(ret){
            mis_counter++;
        }
    }
    
    return 0;
}
