/*
 * File:   newmainXC16.c
 * Author: gregd
 *
 * Created on March 13, 2026, 5:55 PM
 */


#include "xc.h"
#include "functions.h"

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    TRISAbits.TRISA0 = 0; // LD1 in output
    TRISGbits.TRISG9 = 0; // LD2 in output
    LATAbits.LATA0 = 0; // initial value of LD1
    LATGbits.LATG9 = 0; // initial value of LD2
    INTCON2bits.GIE = 1; // allow enabling interrupts
    
    // Assignment 1: LD1 on 500ms - off 500ms (no interrupts), LD2 on 250ms - off 250ms (interrupts)
    
    tmr_setup_period_alternative(250);
    while(1){
        tmr_wait_ms_ultimate(TIMER1,500);
        LATAbits.LATA0 = !LATAbits.LATA0; // toggle LD1
    } 
    
    /*
    // Assignment 2: LD1 on 500ms - off 500ms (no interrupts), LD2 toggles at every T2 (RE8) press (interrupts)
    TRISEbits.TRISE8 = 1; // T2 set in input
    RPINR0bits.INT1R = 88; // external interrupt 1 is mapped to button T2 (RP88)
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt
    IEC1bits.INT1IE = 1; // enabling external interrupt 1
    
    while(1){
        tmr_wait_ms_ultimate(TIMER1,500);
        LATAbits.LATA0 = !LATAbits.LATA0; // toggle LD1
    }
    */
    return 0;
}