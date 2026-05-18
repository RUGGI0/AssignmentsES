#include "xc.h"
#include "functions8.h"

char tx_array[SIZETX];
volatile CircularBuffer tx_buffer;
volatile heartbeat schedInfo[MAX_TASKS];
volatile control_data controlData; // defined in main

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
}

int tmr_wait_period(int timer){
    int temp = 0;
    if(timer == TIMER1){
        if(IFS0bits.T2IF == 1){
            temp = 1;
        }
        while(1){
            if(IFS0bits.T2IF == 1){
                // flag set
                IFS0bits.T2IF = 0; // flag cleared
                break;
            }
        }
    }
    return temp;
}

void scheduler(){
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        schedInfo[i].n++;
        if (schedInfo[i].enable == 1 &&     schedInfo[i].n >= schedInfo[i].N) {
            schedInfo[i].f(schedInfo[i].params); 
            schedInfo[i].n = 0;
        }
    }
}

void task_update_feedback(void* param){
  control_data *cd = (control_data*) param;
  /*
  IEC2bits.QEIIE = 0;
  cd->positionFeedback = (encoder + POSCNT) / GEAR_RATIO / 12.0 * 360.0;
  cd->encoder = encoder + POSCNT;
  IEC2bits.QEIIE = 1;
   * */
}
