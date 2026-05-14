#ifndef FUNCTIONS7_H
#define	FUNCTIONS7_H

#include <xc.h> 

#define TIMER3 3

extern volatile int button_T2_pressed;

// function to set up/change output compare and thus control PWM generation
void OCs_assigning1(int OC1_flag, int OC2_flag, int OC3_flag, int OC4_flag);
void OCs_assigning2(int OC1_flag, int OC2_flag, int OC3_flag, int OC4_flag);
void OCs_assigning3(int OC1_flag, int OC2_flag, int OC3_flag, int OC4_flag);
void assignment1();
void assignment2();
void assignment3(int speed, int yaw);

#endif