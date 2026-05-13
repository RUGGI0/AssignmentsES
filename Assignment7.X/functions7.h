#ifndef FUNCTIONS7_H
#define	FUNCTIONS7_H

#include <xc.h> 

#define TIMER3 3

extern volatile int button_T2_pressed;

// function to set up/change output compare and thus control PWM generation
void OCs_assigning(int OC1_flag, int OC2_flag, int OC3_flag, int OC4_flag);

#endif