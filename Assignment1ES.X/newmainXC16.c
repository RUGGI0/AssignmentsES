/*
 * File:   newmainXC16.c
 * Author: gregorio
 *
 * Created on February 26, 2026, 11:01 AM
 */


#include "xc.h"
#define FCY 3685000UL // cycle frequency for delays (took from Window>Target memory views>configuration bits)
#include <libpic30.h> // delay functions

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // Turn LD1 on + turn off
    
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 1;
    __delay_ms(1000);
    LATAbits.LATA0 = 0;
    
    
    // Turn LD1 on if button T2 is being pressed
    /*
    TRISEbits.TRISE8 = 1;
    TRISAbits.TRISA0 = 0;
    while(1){
        if(PORTEbits.RE8 == 0)
            LATAbits.LATA0 = 1;
        else
            LATAbits.LATA0 = 0;
        __delay_us(500); // delay to avoid busy waiting
    }
    */
    
    // Toggle LD1 for every click of T2 (LD1 starts turned off)
    /*
    TRISEbits.TRISE8 = 1;
    TRISAbits.TRISA0 = 0;
    while(1){
        if(PORTEbits.RE8 == 0) // button clicked
            while(1){
                if(PORTEbits.RE8 == 1){
                    // button released
                    LATAbits.LATA0 = !LATAbits.LATA0; // toggle
                    break;
                } 
            }
    }
    */
    
    return 0;
}