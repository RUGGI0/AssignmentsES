#include "xc.h"
#include "functions7.h"

volatile int button_T2_pressed = 0;

void OCs_assigning1(int OC1_flag, int OC2_flag, int OC3_flag, int OC4_flag){
    if(OC1_flag == 1){
        // PWM-A is activated (which receives from OC1)
        // PWM characteristic:
        // - period: 10kHz
        // - duty cycle: 50% 
        // (necessary to ensure value to wheels is kept constant)
        // Fcy = 72MHz (clock source for OC1) -> 72M/10000 = 7200 (< 65535 -> 16 bit precision)
        // 7200*0.5 = 3600
        OC1R = 3600; // PWM duty cycle
        OC1RS = 7200; // PWM period 10kHz
    }
    else{
        // PWM-A is stopped
        OC1R = 0; // no PWM duty cycle
        OC1RS = 7200; // ensure stopping is done correctly
    }
    
    if(OC2_flag == 1){
        // PWM-B is activated (which receives from OC2)
        // PWM characteristic:
        // - period: 10Hz
        // - duty cycle: 50% 
        // (necessary to ensure value to wheels is kept constant)
        // Fcy = 72MHz (clock source for OC1) -> 72M/10000 = 7200 (< 65535 -> 16 bit precision)
        // 7200*0.5 = 3600
        OC2R = 3600; // PWM duty cycle
        OC2RS = 7200; // PWM period 10kHz
    }
    else{
        // PWM-B is stopped
        OC2R = 0; // no PWM duty cycle
        OC2RS = 7200;
    }
    
    if(OC3_flag == 1){
        // PWM-C is activated (which receives from OC3)
        // PWM characteristic:
        // - period: 10Hz
        // - duty cycle: 50% 
        // (necessary to ensure value to wheels is kept constant)
        // Fcy = 72MHz (clock source for OC1) -> 72M/10000 = 7200 (< 65535 -> 16 bit precision)
        // 7200*0.5 = 3600
        OC3R = 3600; // PWM duty cycle
        OC3RS = 7200; // PWM period 10kHz
        
    }
    else{
        // PWM-C is stopped
        OC3R = 0; // no PWM duty cycle
        OC3RS = 7200;
    }
    
    if(OC4_flag == 1){
        // PWM-D is activated (which receives from OC4)
        // PWM characteristic:
        // - period: 10Hz
        // - duty cycle: 50% 
        // (necessary to ensure value to wheels is kept constant)
        // Fcy = 72MHz (clock source for OC1) -> 72M/10000 = 7200 (< 65535 -> 16 bit precision)
        // 7200*0.5 = 3600
        OC4R = 3600; // PWM duty cycle
        OC4RS = 7200; // PWM period 10kHz
    }
    else{
        // PWM-D is stopped
        OC4R = 0; // no PWM duty cycle
        OC4RS = 7200;
    }
}

void assignment1(){
    int moving = 0;
    while(1){
        if(button_T2_pressed == 1){
            button_T2_pressed = 0;
            if(moving == 0){
                // move forward buggy
                // PWM-A = 0, PWM-B = signal, PWM-C = 0, PWM-D = signal
                OCs_assigning1(0, 1, 0, 1);
                moving = 1;
            }
            else{
                // stop buggy
                // PWM-A = 0, PWM-B = signal, PWM-C = 0, PWM-D = signal
                OCs_assigning1(0, 0, 0, 0);
                moving = 0;
            }
        }
    }    
}

void OCs_assigning2(int OC1_flag, int OC2_flag, int OC3_flag, int OC4_flag){
    if(OC1_flag == 1){
        // PWM-A is activated (which receives from OC1)
        // PWM characteristic:
        // - period: 10kHz
        // - duty cycle: 50% 
        // (necessary to ensure value to wheels is kept constant)
        // Fcy = 72MHz (clock source for OC1) -> 72M/10000 = 7200 (< 65535 -> 16 bit precision)
        // 7200*0.5 = 3600
        OC1R = 3600; // PWM duty cycle
        OC1RS = 7200; // PWM period 10kHz
    }
    else{
        // PWM-A is stopped
        OC1R = 0; // no PWM duty cycle
        OC1RS = 7200; // ensure stopping is done correctly
    }
    
    if(OC2_flag == 1){
        // PWM-B is activated (which receives from OC2)
        // PWM characteristic:
        // - period: 10Hz
        // - duty cycle: 50% 
        // (necessary to ensure value to wheels is kept constant)
        // Fcy = 72MHz (clock source for OC1) -> 72M/10000 = 7200 (< 65535 -> 16 bit precision)
        // 7200*0.7 = 5040
        OC2R = 5400; // PWM duty cycle
        OC2RS = 7200; // PWM period 10kHz
    }
    else{
        // PWM-B is stopped
        OC2R = 0; // no PWM duty cycle
        OC2RS = 7200;
    }
    
    if(OC3_flag == 1){
        // PWM-C is activated (which receives from OC3)
        // PWM characteristic:
        // - period: 10Hz
        // - duty cycle: 50% 
        // (necessary to ensure value to wheels is kept constant)
        // Fcy = 72MHz (clock source for OC1) -> 72M/10000 = 7200 (< 65535 -> 16 bit precision)
        // 7200*0.5 = 3600
        OC3R = 3600; // PWM duty cycle
        OC3RS = 7200; // PWM period 10kHz
        
    }
    else{
        // PWM-C is stopped
        OC3R = 0; // no PWM duty cycle
        OC3RS = 7200;
    }
    
    if(OC4_flag == 1){
        // PWM-D is activated (which receives from OC4)
        // PWM characteristic:
        // - period: 10Hz
        // - duty cycle: 50% 
        // (necessary to ensure value to wheels is kept constant)
        // Fcy = 72MHz (clock source for OC1) -> 72M/10000 = 7200 (< 65535 -> 16 bit precision)
        // 7200*0.5 = 3600
        OC4R = 3600; // PWM duty cycle
        OC4RS = 7200; // PWM period 10kHz
    }
    else{
        // PWM-D is stopped
        OC4R = 0; // no PWM duty cycle
        OC4RS = 7200;
    }
}

void assignment2(){
    int moving = 0;
    while(1){
        if(button_T2_pressed == 1){
            button_T2_pressed = 0;
            if(moving == 0){
                // move forward buggy
                // PWM-A = 0, PWM-B = signal, PWM-C = 0, PWM-D = signal
                OCs_assigning2(0, 1, 0, 1);
                moving = 1;
            }
            else{
                // stop buggy
                // PWM-A = 0, PWM-B = signal, PWM-C = 0, PWM-D = signal
                OCs_assigning2(0, 0, 0, 0);
                moving = 0;
            }
        }
    }    
}

void OCs_assigning3(int OC1_DC, int OC2_DC, int OC3_DC, int OC4_DC){
    
    OC1R = OC1_DC; // PWM duty cycle
    OC1RS = 7200; // PWM period 10kHz
    
    OC2R = OC2_DC; // PWM duty cycle
    OC2RS = 7200; // PWM period 10kHz
    
    OC3R = OC3_DC; // PWM duty cycle
    OC3RS = 7200; // PWM period 10kHz
    
    OC4R = OC4_DC; // PWM duty cycle
    OC4RS = 7200; // PWM period 10kHz   
}

void assignment3(int speed, int yaw){
    int moving = 0;
    int period = 7200;
    int max_DC = 0;
    int other_DC = 0;
    while(1){
        if(button_T2_pressed == 1){
            button_T2_pressed = 0;
            if(moving == 0){
                
                max_DC = speed/100 * period;
                
                if (speed >= 0){
                    
                    if (yaw >= 0){
                        // forward anti-clockwise
                        other_DC = max_DC * (1-(yaw/100));
                        
                        OCs_assigning3(0, other_DC, 0, max_DC);
                    }
                    else{
                        // forward clockwise
                        other_DC = max_DC * (1-(-yaw/100));
                        
                        OCs_assigning3(0, max_DC, 0, other_DC);
                    }
                }
                else{
                    if (yaw >= 0){
                        // backward anti-clockwise
                        other_DC = max_DC * (1-(yaw/100));
                        
                        OCs_assigning3(max_DC, 0, other_DC, 0);
                        
                    }
                    else{
                        // backward clockwise
                        other_DC = max_DC * (1-(-yaw/100));
                        
                        OCs_assigning3(other_DC, 0, max_DC, 0);
                    }
                }
                moving = 1;
            }
            else{
                // STOP
                OCs_assigning3(0, 0, 0, 0);
                moving = 0;
            }
        }
    }    
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
    if(timer == TIMER3){
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
}

// Redefinition of ISR for external interrupt 1 (robust implementation)
void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt(void){
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt
    IPC2bits.T3IP = 0x01; // set priority to one
    IEC0bits.T3IE = 1;
    tmr_setup_period(TIMER3,10);
}

// Redefinition of ISR for timer 3
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void){
    T3CONbits.TON = 0;
    IFS0bits.T3IF = 0; // clearing timer flag
    IEC0bits.T3IE = 0; // disabling timer interrupt
    
    if(PORTEbits.RE8 == 0){
        button_T2_pressed = 1;
    }
}