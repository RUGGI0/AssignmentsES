/*
 * File:   newmainXC16.c
 * Author: gregorio
 *
 * Created on February 26, 2026, 11:01 AM
 */


#include "xc.h"

void count(int start){
    while(start > 0){
        start--;
        start++;
        start--;
        start++;
        start--;
    }
}

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // Turn LD1 on + turn off
    /*
    int start = 100000;
    TRISAbits.TRISA0 = 0;
    
    LATAbits.LATA0 = 1;
    for(int a = 10000; a > 0; a--){
        count(start); 
    }
    
    LATAbits.LATA0 = 0;
    */
    
    
    // Turn LD1 on if button T2 is being pressed
    /*
    TRISEbits.TRISE8 = 1;
    TRISAbits.TRISA0 = 0;
    TRISEbits.TRISE9 = 1;
    TRISGbits.TRISG9 = 0;
    while(1){
        if(PORTEbits.RE8 == 0)
            LATAbits.LATA0 = 1;
        else
            LATAbits.LATA0 = 0;
        __delay_us(500); // delay to avoid busy waiting
        if(PORTEbits.RE9 == 0) // button clicked
            while(1){
                if(PORTEbits.RE9 == 1){
                    // button released
                    LATGbits.LATG9 = !LATGbits.LATG9; // toggle
                    break;
                } 
            }
    }
     * */
    
    // Toggle LD1 for every click of T2 (LD1 starts turned off)
    
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
    
  
    
    
    return 0;
}
