#include "xc.h"

void device_init(){
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // ----* Configure lights *---- //
    TRISAbits.TRISA0 = 0; // LD1
    LATAbits.LATA0 = 0;
    
    TRISBbits.TRISB8 = 0; // left-side lights
    LATBbits.LATB8 = 0;
    
    TRISFbits.TRISF1 = 0; // right-side lights
    LATFbits.LATF1 = 0;
    
    TRISFbits.TRISF0 = 0; // break lights
    LATFbits.LATF0 = 0;
    
    TRISGbits.TRISG1 = 0; // low lights
    LATGbits.LATG1 = 0;
    
    TRISAbits.TRISA7 = 0; // beam headlights
    LATAbits.LATA7 = 0;
    
    INTCON2bits.GIE = 1; // allow enabling interrupts
    
    // ----* Configure UART *---- //
    TRISDbits.TRISD0 = 0; // Tx in output
    TRISDbits.TRISD11 = 1; // Rx in input
    
    // Mapping pins
    RPOR0bits.RP64R = 1; // RD0 (RP64) mapped to UART1 Tx
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
    
    // Tx interrupt set up
    IFS0bits.U1TXIF = 0; // clearing Tx interrupt flag
    IPC3bits.U1TXIP = 1; // priority for Tx
    U1STAbits.UTXISEL0 = 0; // Tx interrupt triggers if a char is sent to U1TXREG,
    U1STAbits.UTXISEL1 = 0; // meaning U1TXREG has at least one slot empty ->
    // it will trigger right after enabling U1TX
    
    U1MODEbits.UARTEN = 1; // enable UART1
    U1STAbits.UTXEN = 1; // enable U1TX (transmission)
    
    // ----* Configure SPI *---- //
    TRISAbits.TRISA1 = 1; // MISO (input)
    TRISFbits.TRISF12 = 0; // SCK (clock)(output)
    TRISFbits.TRISF13 = 0; // MOSI (output)
    TRISBbits.TRISB3 = 0; // CS1 (output)
    TRISBbits.TRISB4 = 0; // CS2 (output)
    TRISDbits.TRISD6 = 0; // CS3 (output)
    
    // pin mapping
    RPINR20bits.SDI1R = 17; // RPI17 -> MISO (SDI1)
    RPOR11bits.RP108R = 0b000110; // RF12 -> SCK1 (clock)
    RPOR12bits.RP109R = 0b000101; // RF13 -> MOSI (SDO1)
    
    // clearing SPI interrupt
    IFS0bits.SPI1IF = 0;
    IEC0bits.SPI1IE = 0;
    
    SPI1CON1bits.DISSCK = 0;
    SPI1CON1bits.DISSDO = 0;
    SPI1CON1bits.SMP = 0;
    SPI1CON1bits.CKE = 0;
    SPI1CON1bits.CKP = 1;
    
    SPI1CON1bits.MSTEN = 1; // master mode on
    SPI1CON1bits.PPRE = 2; // primary prescaler 4:1
    SPI1CON1bits.SPRE = 6; // secondary prescaler 2:1
    // 9MHZ clock for SPI (Fcy/8 -> 72MHz/8)
    SPI1CON1bits.MODE16 = 0; // 8-bit data communication
    SPI1STATbits.SPIEN = 1; // enable SPI
    
    // deselecting chips
    LATBbits.LATB3 = 1; // accelerometers (off)
    LATBbits.LATB4 = 1; // gyroscope (off)
    LATDbits.LATD6 = 1; // magnetometer (off)
    
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
    
    // ---* Configure ADC *--- //
    // - using ADC module 1 (registers 'AD1...')
    // - AVdd = Vrefh = 3.6V (considering 3.3V ?) / Vinl = AVss = Vrefl = 0v -> page 558
    AD1CON1bits.AD12B = 0; // 10-bit ADC mode (4 channels available) -> conversion time = 12 * Tad
    AD1CON3bits.ADRC = 0; // using internal clock cycle (Tcy) for ADC module
    
    // Tad definition: Tad = (ADCS + 1) * Tcy, Tad must be > 75ns for 10-bit ADC,
    // since Fcy = 72MHz -> Tcy = 13.89ns
    AD1CON3bits.ADCS = 8; // Tad = 9 * 13.89 = 125ns
    AD1CON1bits.ASAM = 1; // automatic sampling (starts at the end of previous conversion phase)
    AD1CON3bits.SAMC = 16; // sample time ends after 16 Tad (2 us)
    AD1CON1bits.SSRC = 2; // automatic conversion triggered by Timer 3 expiration
    
    // sample time + conversion time = Tad*16 + Tad*12 = 3.5 us -> 3.5mil/s
    // required sampling/conversion is 1000/s
    
    AD1CON2bits.SMPI = 1; // ADC interrupt gets triggered after two conversions (AN5 AN11)
    IFS0bits.AD1IF = 0;
    IPC3bits.AD1IP = 1;
    IEC0bits.AD1IE = 1; // enabling ADC interrupt
    
    AD1CON2bits.CHPS = 0; // activating CH0 (1 channel mode -> CH0 needed to access AN11 and AN5 -> using scan mode)
    AD1CON1bits.FORM = 0; // output format: unsigned integer (10-bit -> 0-1023)
    
    AD1CON2bits.CSCNA = 1; // scan mode active
    
    // selecting analogical pins to be scanned 
    AD1CSSLbits.CSS5 = 1; // AN5 -> output from IR Sensor board (Volts)
    AD1CSSLbits.CSS11 = 1; // AN11 -> battery voltage (Volts)
    
    ANSELBbits.ANSB11 = 1; // activating AN11 for battery sensing
    TRISBbits.TRISB11 = 1;
    
    ANSELBbits.ANSB14 = 1; // activating AN14 for IR sensor voltage (buggy micro BUS)
    TRISBbits.TRISB14 = 1;
    
    TRISBbits.TRISB9 = 0; // pin to enable IR Sensor board (buggy micro BUS)
    LATBbits.LATB9 = 1;
    
    AD1CON1bits.ADON = 1; // activating ADC module 1

}