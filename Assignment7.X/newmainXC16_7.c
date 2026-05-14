#include "xc.h"
#include "functions7.h"

int main(void) {
    
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // *---- Part 1 ----* //
    // using output compare to generate a PWM signal on the corresponding pins
    // PWM to move wheels must have a duty cycle of 40% at 1000Hz
    
    // PWM pins remap (values for OCx functionalities at page 215 of datasheet)
    RPOR0bits.RP65R = 0b010000; // RD1 -> OC1
    RPOR1bits.RP66R = 0b010001; // RD2 -> OC2
    RPOR1bits.RP67R = 0b010010; // RD3 -> OC3
    RPOR2bits.RP68R = 0b010011; // RD4 -> OC4
    
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
    
    // *--- Output compare set up ---* //
    // - using 4 modules to control 4 PWM signals
    // - OCxR: primary OCx module register (used for duty cycle of OCx pin)
    // - OCxRS: secondary OCx module register (used for period of OCx pin)
    
    // OC1 module
    OC1R = 0; // initial value -> duty cycle zero (no movement)
    OC1RS = 7200; // initial  value of frequency (look OCs_assigning function for details)
    OC1CON1bits.OCTSEL = 0b111; // internal clock (Fp) as clock source of OC1 module
    OC1CON2bits.SYNCSEL = 0b11111; // No Sync source, however OC1TMR resets if = OC1RS, and OC 1 module uses its own Sync signal
    OC1CON1bits.OCM = 0b110; // OC modality is Edge-Aligned: Output (OC1) is set high when OC1TMR = 0 and low when OC1TMR = OC1R
    
    // OC2 module
    OC2R = 0; // initial value -> duty cycle zero (no movement)
    OC2RS = 7200; // initial value of frequency (look OCs_assigning function for details)
    OC2CON1bits.OCTSEL = 0b111; // internal clock (Fp) as clock source of OC2 module
    OC2CON2bits.SYNCSEL = 0b11111; // No Sync source, however OC2TMR resets if = OC2RS, and OC 2 module uses its own Sync signal
    OC2CON1bits.OCM = 0b110; // OC modality is Edge-Aligned: Output (OC2) is set high when OC2TMR = 0 and low when OC2TMR = OC2R
    
    // OC3 module
    OC3R = 0; // initial value -> duty cycle zero (no movement)
    OC3RS = 7200; // initial value of frequency (look OCs_assigning function for details)
    OC3CON1bits.OCTSEL = 0b111; // internal clock (Fp) as clock source of OC3 module
    OC3CON2bits.SYNCSEL = 0b11111; // No Sync source, however OC3TMR resets if = OC3RS, and OC 3 module uses its own Sync signal
    OC3CON1bits.OCM = 0b110; // OC modality is Edge-Aligned: Output (OC3) is set high when OC1TMR = 0 and low when OC3TMR = OC3R
    
    // OC4 module
    OC4R = 0; // initial value -> duty cycle zero (no movement)
    OC4RS = 7200; // initial value of frequency (look OCs_assigning function for details)
    OC4CON1bits.OCTSEL = 0b111; // internal clock (Fp) as clock source of OC4 module
    OC4CON2bits.SYNCSEL = 0b11111; // No Sync source, however OC4TMR resets if = OC4RS, and OC 4 module uses its own Sync signal
    OC4CON1bits.OCM = 0b110; // OC modality is Edge-Aligned: Output (OC4) is set high when OC4TMR = 0 and low when OC4TMR = OC4R
    
    INTCON2bits.GIE = 1; // allow enabling interrupts
    TRISEbits.TRISE8 = 1; // T2 in input
    
    // remapping button
    RPINR0bits.INT1R = 88; // external interrupt 1 is mapped to button T2 (RP88)
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt 1
    IPC5bits.INT1IP = 1; // priority level 1
    INTCON2bits.INT1EP = 1; // triggers on falling edge (1->0)
    IEC1bits.INT1IE = 1; // enabling external interrupt 1
    
    T3CONbits.TON = 0;
    IFS0bits.T3IF = 0; // clearing timer flag
    IEC0bits.T3IE = 0; // disabling timer interrupt
    
    //OCs_assigning1(0,0,0,0); // ensure all value are zeroed
    
    // assignment1();
    // assignment2();
    // assignment3(100, 100);
    
    
    
    return 0;
}
