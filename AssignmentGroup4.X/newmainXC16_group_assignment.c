/*
 * File:   newmainXC16_group_assignment.c
 * Author: gregd
 *
 * Created on April 27, 2026, 1:55 PM
 */


#include "xc.h"
#include <math.h>
#include "assignment_functions.h"

int main(void) {
    
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // ----* Configure LD2 *---- //
    TRISGbits.TRISG9 = 0; // LD2 in output
    LATGbits.LATG9 = 0; // initial value of LD2
    
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
    SPI1CON1bits.CKE = 1;
    SPI1CON1bits.CKP = 0;
    
    SPI1CON1bits.MSTEN = 1; // master mode on
    SPI1CON1bits.PPRE = 2; // primary prescaler 4:1
    SPI1CON1bits.SPRE = 6; // secondary prescaler 2:1
    // 9MHZ clock for SPI (Fcy/8 -> 72MHz/8)
    SPI1CON1bits.MODE16 = 0; // 8-bit data communication
    SPI1STATbits.SPIEN = 1; // enable SPI
    
    // selecting chip (magnetometer)
    LATBbits.LATB3 = 1; // accelerometers (off)
    LATBbits.LATB4 = 1; // gyroscope (off)
    LATDbits.LATD6 = 1; // magnetometer (off)
    
    int yy;
    int ret = -1;
    int acc_x = 0;
    int acc_y = 0;
    int acc_z = 0;
    int roll = 0, pitch = 0;
    int cycle_counter = 0;
    char c1 = '-';
    char c2 = '-';
    char c3 = '-';
    char c4 = '-';
    char c5 = '-';
    char c6 = '-';
    char c7 = '-';
    volatile CircularBuffer rx_buffer;
    volatile CircularBuffer tx_buffer;
    
    buffer_init(&rx_buffer);
    buffer_init(&tx_buffer);
    
    tmr_setup_period(TIMER1,10);
    
    while(1){
        algorithm();
        
        while(buffer_is_empty(&rx_buffer) == 0){
            char c;
            int temp;
            buffer_read(&rx_buffer, &c);
            c7 = c6;
            c6 = c5;
            c5 = c4;
            c4 = c3;
            c3 = c2;
            c2 = c1;
            c1 = c;
            if(c7 == '$'){
                if(c6 == 'B' && c5 == 'W' && c4 == ',' && c1 == '*'){
                    temp = (c3 - '0')*10 + (c2 - '0');
                    if(temp >= 8 && temp <= 15){
                        set_accelerometer_bandwidth(temp);
                    }
                    else{
                        // send error
                        send_error_to_uart();
                    }
                }
                if(c6 == 'H' && c5 == 'Z' && c4 == ',' && c1 == '*'){
                    temp = (c3 - '0')*10 + (c2 - '0');
                    if(temp == 0 || temp == 1 || temp == 2 || temp == 5 || temp == 10){
                        yy = temp;
                    }
                    else{
                        // send error
                        send_error_to_uart();
                    }
                }
            }
        }
        
        if(cycle_counter % 50 == 0){
            LATGbits.LATG9 = !LATGbits.LATG9; // toggle LD2
        }
        
        if(cycle_counter % 2 == 0){
            // every 50Hz, and triggers at start
            
            // acquiring x-axis of accelerometer
            acc_x = get_accelerometer_value(0x02);
            
            // acquiring y-axis of accelerometer
            acc_y = get_accelerometer_value(0x04);
            
            // acquiring z-axis of accelerometer
            acc_z = get_accelerometer_value(0x06);
            
            // compute roll and pitch angles
            roll = atan2(acc_y, acc_z);
            pitch = atan2(-acc_x,sqrt(acc_y*acc_y + acc_z*acc_z));
        }
        
        if(yy != 0 && cycle_counter % (1000/yy) == 0){
            // send the x, y, z accelerations
            send_accelerometer_values_to_uart(acc_x, acc_y, acc_z);
        }
        
        if(cycle_counter % 200 == 0){
            // frequency of 5Hz to send the computed angles
            send_roll_pitch_to_uart(roll, pitch);
        }
        
        if(cycle_counter == 1000){
            cycle_counter = 0;
        }
        
        cycle_counter++;
        ret = tmr_wait_period(TIMER1);
    }
    
    
    return 0;
}
