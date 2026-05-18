#include "xc.h"
#include "functions8.h"

int main(void) {
    
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    INTCON2bits.GIE = 1; // allow enabling interrupts
    
    // ----* Configure Button *---- //
    TRISEbits.TRISE8 = 1; // T2 in input (no interrupt)
    
    // ----* Configure Buggy lights *---- //
    TRISBbits.TRISB8 = 0; // Left lights
    TRISFbits.TRISF1 = 0; // Right lights
    TRISFbits.TRISF0 = 0; // Brake lights
    TRISGbits.TRISG1 = 0; // Low intensity lights
    
    // ----* Configure UART *---- //
    TRISDbits.TRISD11 = 1; // Rx in input
    
    // Mapping pins
    RPINR18bits.U1RXR = 75; // UART1 Rx mapped to RD11

    // configuring UART1 to have baud rate = 9600 -> [72 000 000/(16*9600)] - 1
    U1BRG = 468; // 467.75 baud rate = 9600
    U1MODEbits.STSEL = 0; // 1 stop bit
    U1MODEbits.PDSEL = 0; // no parity bits - 8 data bits
    U1MODEbits.ABAUD = 0; // no auto baud rate
    U1MODEbits.BRGH = 0; // low-speed mode
    
    // Rx interrupt set up
    IFS0bits.U1RXIF = 0; // clearing Rx interrupt flag
    IPC2bits.U1RXIP = 1; // priority for Rx
    U1STAbits.URXISEL = 0; // interrupt if a character is received
    IEC0bits.U1RXIE = 1; // enabling interrupt for UART1 Rx
    
    // *--- Output compare set up ---* //
    // - using 4 modules to control 4 PWM signals
    // - OCxR: primary OCx module register (used for duty cycle of OCx pin)
    // - OCxRS: secondary OCx module register (used for period of OCx pin)
    
    // PWM pins remap (values for OCx functionalities at page 215 of datasheet)
    RPOR0bits.RP65R = 0b010000; // RD1 -> OC1
    RPOR1bits.RP66R = 0b010001; // RD2 -> OC2
    RPOR1bits.RP67R = 0b010010; // RD3 -> OC3
    RPOR2bits.RP68R = 0b010011; // RD4 -> OC4
    
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
    
    // OC1 module
    OC1CON1bits.OCM = 0; // ensure disabled for safety
    OC1R = 0; // initial value -> duty cycle zero (no movement)
    OC1RS = 7200; // initial  value of frequency (look OCs_assigning function for details)
    OC1CON1bits.OCTSEL = 0b111; // internal clock (Fp) as clock source of OC1 module
    OC1CON2bits.SYNCSEL = 0b11111; // No Sync source, however OC1TMR resets if = OC1RS, and OC 1 module uses its own Sync signal
    OC1CON1bits.OCM = 0b110; // OC modality is Edge-Aligned: Output (OC1) is set high when OC1TMR = 0 and low when OC1TMR = OC1R
    
    // OC2 module
    OC2CON1bits.OCM = 0; // ensure disabled for safety
    OC2R = 0; // initial value -> duty cycle zero (no movement)
    OC2RS = 7200; // initial value of frequency (look OCs_assigning function for details)
    OC2CON1bits.OCTSEL = 0b111; // internal clock (Fp) as clock source of OC2 module
    OC2CON2bits.SYNCSEL = 0b11111; // No Sync source, however OC2TMR resets if = OC2RS, and OC 2 module uses its own Sync signal
    OC2CON1bits.OCM = 0b110; // OC modality is Edge-Aligned: Output (OC2) is set high when OC2TMR = 0 and low when OC2TMR = OC2R
    
    // OC3 module
    OC3CON1bits.OCM = 0; // ensure disabled for safety
    OC3R = 0; // initial value -> duty cycle zero (no movement)
    OC3RS = 7200; // initial value of frequency (look OCs_assigning function for details)
    OC3CON1bits.OCTSEL = 0b111; // internal clock (Fp) as clock source of OC3 module
    OC3CON2bits.SYNCSEL = 0b11111; // No Sync source, however OC3TMR resets if = OC3RS, and OC 3 module uses its own Sync signal
    OC3CON1bits.OCM = 0b110; // OC modality is Edge-Aligned: Output (OC3) is set high when OC1TMR = 0 and low when OC3TMR = OC3R
    
    // OC4 module
    OC4CON1bits.OCM = 0; // ensure disabled for safety
    OC4R = 0; // initial value -> duty cycle zero (no movement)
    OC4RS = 7200; // initial value of frequency (look OCs_assigning function for details)
    OC4CON1bits.OCTSEL = 0b111; // internal clock (Fp) as clock source of OC4 module
    OC4CON2bits.SYNCSEL = 0b11111; // No Sync source, however OC4TMR resets if = OC4RS, and OC 4 module uses its own Sync signal
    OC4CON1bits.OCM = 0b110; // OC modality is Edge-Aligned: Output (OC4) is set high when OC4TMR = 0 and low when OC4TMR = OC4R
    
    // Initialization //
    int ret = 0;
    // contolData defined in here...
    
    schedInfo[0].n = 0;
    schedInfo[0].N = 0;
    schedInfo[0].f = task_update_feedback;
    schedInfo[0].params = (void*)(&controlData);
    schedInfo[0].enable = 1;
    
    schedInfo[1].n = 0;
    schedInfo[1].N = 0;
    schedInfo[1].f = task_update_feedback;
    schedInfo[1].params = (void*)(&controlData);
    schedInfo[1].enable = 1;
    
    schedInfo[2].n = 0;
    schedInfo[2].N = 0;
    schedInfo[2].f = task_update_feedback;
    schedInfo[2].params = (void*)(&controlData);
    schedInfo[2].enable = 1;
    
    schedInfo[3].n = 0;
    schedInfo[3].N = 0;
    schedInfo[3].f = task_update_feedback;
    schedInfo[3].params = (void*)(&controlData);
    schedInfo[3].enable = 1;
    
    tmr_setup_period(TIMER1,1);
    
    while(1){
        scheduler();
        ret = tmr_wait_period(TIMER1);
    }
    
    return 0;
}
