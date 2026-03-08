/*
 * File:   newmainXC16.c
 * Author: gregd
 *
 * Created on March 6, 2026, 5:23 PM
 */


#include "xc.h"
#include "timer.h"

int main(void) {
    TRISAbits.TRISA0 = 0;// LD1
    LATAbits.LATA0 = 0;
    
    // Assignment 1/3
    
    tmr_setup_period(TIMER1, 200);
    // may need to add instructions to check whether flag has been already risen
    while(1){
        LATAbits.LATA0 = !LATAbits.LATA0; // toggle LD1
        tmr_wait_period(TIMER1);
    }
    
    
    // Assignment 2/3
    
    while(1){
        LATAbits.LATA0 = 1;
        tmr_wait_ms(TIMER2, 20);
        LATAbits.LATA0 = 0;
        tmr_wait_ms(TIMER2, 200);
    }
    
    
    //Assignment 3/3
    TRISGbits.TRISG9 = 0; // LD2
    LATGbits.LATG9 = 0;
    int ret = -1;
    tmr_setup_period(TIMER1, 200);
    int delay = 2000; // try 50, 200, 2000 delay
    while(1){
        tmr_wait_ms_alternative(TIMER2,delay); // simulating some operation
        LATAbits.LATA0 = !LATAbits.LATA0; // toggle LD1
        ret = tmr_wait_period_alternative(TIMER1);
        if(ret)
            // TIMER1 checked too late
            LATGbits.LATG9 = 1; // lD2 on
        else
            // TIMER1 checked correctly
            LATGbits.LATG9 = 0; // lD2 off
    }
    
    return 0;
}