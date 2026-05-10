#include "xc.h"
#include "functions6.h"

int main(void) {
    
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    INTCON2bits.GIE = 1; // allow enabling interrupts
    
    // ----* Assignment Part 1: reading battery value (AN11) *---- //
    //part_1_assignment();

    // ----* Assignment Part 2: reading IR Sensor distance (AN5) *---- //
    //part_2_assignment();
    
    // ----* Assignment Part 3: reading battery value and IR Sensor distance *---- //
    part_3_assignment();
        
    return 0;
}
