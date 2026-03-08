/*
 * File:   timer.c
 * Author: gregd
 *
 * Created on March 6, 2026, 5:27 PM
 */


#include "xc.h"
#include "timer.h"

// Function to setup timer counting up to 200ms (max value)
// Assignment 1/3
void tmr_setup_period(int timer, int ms){
    // Highest value is supported with 16 bits timer:
    // if ms == 200 and Fcy = 72MHz -> in 200ms 14 400 000 clock steps ->
    // not countable with 16 bits(TMR1) (65535 values) -> prescaler = 256:
    // 14 400 000/256 = 56 250 steps
    // Better take the lowest prescalar possible
    long Fcy = 72000000; // dsPIC33EP512MU810
    long cycles = Fcy / 1000;
    cycles *= ms; // avoids overflow with Fcy*ms/1000
    if(timer == 1){
        T1CONbits.TCS = 0;
        T1CONbits.TGATE = 0;
        T1CONbits.TON = 0; // ensure timer is off
        TMR1 = 0; // reset timer 1
        IFS0bits.T1IF = 0; // clear stale period flag before restarting
        if(cycles <= 65535){
            T1CONbits.TCKPS = 0b00; // 1:1 prescaler
            PR1 = cycles;
        }
        else if(cycles/8 <= 65535){
            T1CONbits.TCKPS = 0b01; // 1:8 prescaler
            PR1 = cycles/8;
        }
        else if(cycles/64 <= 65535){
            T1CONbits.TCKPS = 0b10; // 1:64 prescaler
            PR1 = cycles/64;
        }
        else{
            T1CONbits.TCKPS = 0b11; // 1:256 prescaler
            PR1 = cycles/256;
        }
        T1CONbits.TON = 1;
        return;
    }
    else if(timer == 2){
        T2CONbits.TCS = 0;
        T2CONbits.TGATE = 0;
        T2CONbits.TON = 0; // ensure timer is off
        TMR2 = 0; // reset timer 2
        IFS0bits.T2IF = 0; // clear stale period flag before restarting
        if(cycles <= 65535)
            PR2 = cycles;
        else if(cycles/8 <= 65535){
            T2CONbits.TCKPS = 1; // 1:8 prescaler
            PR2 = cycles/8;
        }
        else if(cycles/64 <= 65535){
            T2CONbits.TCKPS = 0b10; // 1:64 prescaler
            PR2 = cycles/64;
        }
        else{
            T2CONbits.TCKPS = 0b11; // 1:256 prescaler
            PR2 = cycles/256;
        }
        T2CONbits.TON = 1;
        return;
    }
}

// Function to expire the passed timer 
// Assignment 1/3
void tmr_wait_period(int timer){
    if(timer == 1){
        while(1){
            if(IFS0bits.T1IF == 1){
                // flag set
                IFS0bits.T1IF = 0; // flag cleared
                TMR1 = 0; // reset timer 1
                break;
            }
        }
    }
    else if(timer == 2){
        while(1){
            if(IFS0bits.T2IF == 1){
                IFS0bits.T2IF = 0;
                TMR2 = 0;
                break;
            }
        }
    }
}

// Function to setup and expire a timer with provided ms (max 200) 
// Assignment 2/3
void tmr_wait_ms(int timer, int ms){
    tmr_setup_period(timer, ms);
    if(timer == 1){
        while(1){
            if(IFS0bits.T1IF == 1){
                // flag set
                T1CONbits.TON = 0; // stop timer
                IFS0bits.T1IF = 0; // flag cleared
                TMR1 = 0; // reset timer 1
                break;
            }
        }
    }
    else if(timer == 2){
        while(1){
            if(IFS0bits.T2IF == 1){
                T2CONbits.TON = 0;
                IFS0bits.T2IF = 0;
                TMR2 = 0;
                break;
            }
        }
    }
}

// Variation of void tmr_wait_period(int timer) that adds a control variable
// Assignment part 3/3
int tmr_wait_period_alternative(int timer){
    int temp = 0;
    if(timer == 1){
        if(IFS0bits.T1IF == 1)
            temp = 1;
        while(1){
            if(IFS0bits.T1IF == 1){
                // flag set
                IFS0bits.T1IF = 0; // flag cleared
                TMR1 = 0; // reset timer 1
                break;
            }
        }
    }
    else if(timer == 2){
        if(IFS0bits.T2IF == 1)
            temp = 1;
        while(1){
            if(IFS0bits.T2IF == 1){
                IFS0bits.T2IF = 0;
                TMR2 = 0;
                break;
            }
        }
    }
    return temp;
}

// Variation of void tmr_wait_ms(int timer, int ms) to allow any value of ms
// Assignment 3/3
void tmr_wait_ms_alternative(int timer, int ms){
    tmr_setup_period_alternative(timer, ms);
    while(1){
        if(IFS0bits.T3IF == 1){
            // flag set
            T2CONbits.TON = 0;
            IFS0bits.T3IF = 0;
            // must clear both timers
            TMR2 = 0;
            TMR3 = 0;
            break;
        }
    }
}

// Function to setup a 32-bits timer (timer2 and timer3)
// Assignment 3/3
void tmr_setup_period_alternative(int timer,int ms){
    uint32_t Fcy = 72000000; // dsPIC33EP512MU810
    uint32_t cycles = Fcy / 1000;
    cycles *= ms; // avoids overflow with Fcy*ms/1000
    T2CONbits.TON = 0; // ensure timer is off
    T3CONbits.TON = 0;
    T2CONbits.T32 = 1; // enabling 32-bit timer
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
    T2CONbits.TCKPS = 0; // 1:1 prescaler otherwise overflow
    TMR2 = 0; // reset timer 2
    TMR3 = 0; // reset timer 3
    IFS0bits.T2IF = 0; // clear stale period flag before restarting
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 0; // polling mode: interrupt not required
    //IPC2bits.T3IP = 0x01; // set priority level for timer 3
    
    PR2 = (uint16_t)(cycles & 65535); // lsw (taking least significant 16 bits)
    PR3 = (uint16_t)(cycles >> 16); // msw (taking most significant 16 bits)
    
    T2CONbits.TON = 1;
    return;
}