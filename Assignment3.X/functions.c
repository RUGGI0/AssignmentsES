/*
 * File:   functions.c
 * Author: gregd
 *
 * Created on March 13, 2026, 5:56 PM
 */


#include "xc.h"
#include "functions.h"

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

// Function to setup and expire a timer with provided ms (max 200)
void tmr_wait_ms(int timer, int ms){
    tmr_setup_period(timer, ms);
    if(timer == TIMER1){
        while(1){
            if(IFS0bits.T1IF == 1){
                // flag set
                T1CONbits.TON = 0; // stop timer
                IFS0bits.T1IF = 0; // flag cleared
                break;
            }
        }
    }
    else if(timer == TIMER2){
        while(1){
            if(IFS0bits.T2IF == 1){
                T2CONbits.TON = 0;
                IFS0bits.T2IF = 0;
                break;
            }
        }
    }
}

// Function to wait an arbitrary amount of time (ms)
void tmr_wait_ms_ultimate(int timer, int ms){
    int n = 100;
    int n_calls = ms/n;
    int remainder = ms % n;
    while(1){
       if(n_calls == 0){
          break;
       }
       tmr_wait_ms(timer, n); // max 16 bit timer
       n_calls--;
    }
    if(remainder != 0){
        tmr_wait_ms(timer, remainder);
    }
}

// Function to setup a 32-bits timer (timer2 and timer3)
void tmr_setup_period_alternative(int ms){
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
    PR2 = (uint16_t)(cycles & 65535); // lsw (taking least significant 16 bits)
    PR3 = (uint16_t)(cycles >> 16); // msw (taking most significant 16 bits)
    
    IFS0bits.T2IF = 0; // clear interrupt flag
    IFS0bits.T3IF = 0; // clear interrupt flag
    IEC0bits.T3IE = 1; // enable interrupt
    IPC2bits.T3IP = 0x01; // set priority level for timer 3
    
    T2CONbits.TON = 1;
    return;
}

// Redefinition of ISR for timer 3
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void){
    IFS0bits.T3IF = 0; // clearing timer flag
    LATGbits.LATG9 = !LATGbits.LATG9; // toggle LD2
}

// Redefinition of ISR for external interrupt 1
void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt(void){
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt
    LATGbits.LATG9 = !LATGbits.LATG9; // toggle LD2
}

// Redefinition of ISR for timer 1
/* alternative of assignment 1 -> need to enable interrupt timer 1 and change tmr_wait_ms
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void){
    IFS0bits.T1IF = 0; // clearing timer flag
    LATGbits.LATG9 = !LATGbits.LATG9; // toggle LD2
}
 * */

