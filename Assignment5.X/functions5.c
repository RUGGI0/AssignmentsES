/*
 * File:   functions5.c
 * Author: gregd
 *
 * Created on April 12, 2026, 6:04 PM
 */


#include "xc.h"
#include "functions5.h"

void assignment_1(){
    
    unsigned int output = 0;
    unsigned int input = 0;
    
    // --- Switching magnetometer to Sleep mode ---
    tmr_wait_ms(TIMER1, 3); // wait 3ms to reach Suspended mode (pag 21 - notes 7)
    input = 0x4B; // register address used to change from Suspended mode to Sleep mode
    // no need to add additional bits since it's a writing operation (initial bit = 0)
    output = spi_write(input); // sending address to write 
    input = 0x01; // data to write (changing from Suspended mode to Sleep mode)
    output = spi_write(input);
    
    // --- Switching magnetometer to Active mode ---
    tmr_wait_ms(TIMER1, 3); // wait 3ms to reach Sleep mode (pag 22 - notes 7) 
    input = 0x4C; // register address used to change from Sleep mode to Active mode
    // no need to add additional bits since it's a writing operation (initial bit = 0)
    output = spi_write(input); // sending address to write 
    input = 0x00; // data to write (changing from Sleep mode to Active mode)
    output = spi_write(input);
    
    // --- Reading Chip ID ---
    tmr_wait_ms(TIMER1, 3); // wait 3ms to reach Active mode (not sure) 
    input = 0x40; // register address with Chip ID info
    input = input | 0x80; // setting MSB to 1 since it's a reading operation
    output = spi_write(input); // sending address to write 
    input = 0x00; // dummy input to retrieve info (Chip ID)
    output = spi_write(input);
    
    LATCbits.LATC1 = 0; // magnetometer (off)
    
    // --- Sending Chip ID to UART ---
    while(U1STAbits.UTXBF == 1); // Tx buffer is full -> wait
    U1TXREG = output; 
}

void assignment_2(){
    
    unsigned int output_1 = 0;
    unsigned int output_2 = 0;
    unsigned int input = 0;
    unsigned int res = 0;
    char msg[SIZE] = "";
    
    // --- Switching magnetometer to Sleep mode ---
    tmr_wait_ms(TIMER1, 3); // wait 3ms to reach Suspended mode (pag 21 - notes 7)
    input = 0x4B; // register address used to change from Suspended mode to Sleep mode
    // no need to add additional bits since it's a writing operation (initial bit = 0)
    output_1 = spi_write(input); // sending address to write 
    input = 0x01; // data to write (changing from Suspended mode to Sleep mode)
    output_1 = spi_write(input);
    
    // --- Switching magnetometer to Active mode ---
    tmr_wait_ms(TIMER1, 3); // wait 3ms to reach Sleep mode (pag 22 - notes 7) 
    input = 0x4C; // register address used to change from Sleep mode to Active mode
    // no need to add additional bits since it's a writing operation (initial bit = 0)
    output_1 = spi_write(input); // sending address to write 
    input = 0x00; // data to write (changing from Sleep mode to Active mode)
    output_1 = spi_write(input);
    
    // --- Reading x-axis of magnetometer ---
    while(1){
        tmr_wait_ms(TIMER1, 3); // wait 3ms to reach Active mode (not sure) 
        input = 0x42; // register address with x-axis info (first 5 bits)
        input = input | 0x80; // setting MSB to 1 since it's a reading operation
        output_1 = spi_write(input); // sending address to write 
        input = 0x00; // dummy input to retrieve info (x-axis)
        output_1 = spi_write(input); // 5 LSBs of x-axis
        output_2 = spi_write(input); // 8 MSBs of x-axis (automatically reads next register -> 0x43)

        output_1 = output_1 & 0x00F8; // clearing last 3 bits
        output_2 = output_2 << 8; // shifting of 8 slots the 8 MSB of x-axis
        res = output_2 | output_1; // or between two parts
        res = res / 8; // safe shifting 13 bits to the left by 3 slots -> 13 bits
        
        tmr_wait_ms(TIMER1, 97);
        sprintf(msg, "MAGX=%d", res);
        for(int i = 0;i<SIZE;i++){
                if(msg[i] == '\0'){
                    break;
                }
                while(U1STAbits.UTXBF == 1); // Tx buffer is full -> wait
                U1TXREG = msg[i];
            }
    }
    
}

unsigned int spi_write(unsigned int data){
    // writing data provided
    while(SPI1STATbits.SPITBF == 1); // wait until Tx buffer is empty
    SPI1BUF = data; // send data (could be greater than a byte)
    
    // reading answer
    while(SPI1STATbits.SPIRBF == 0); // wait until Rx buffer is full
    data = SPI1BUF;
    return data;
}

// Function to setup timer counting up to 200ms (max value)
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
    if(timer == TIMER1){
        T1CONbits.TCS = 0;
        T1CONbits.TGATE = 0;
        T1CONbits.TON = 0; // ensure timer is off
        TMR1 = 0; // reset timer 1
        IFS0bits.T1IF = 0; // clear state period flag before restarting
        if(cycle_case == 0){
            T1CONbits.TCKPS = 0b00; // 1:1 prescaler
        }
        else if(cycle_case == 1){
            T1CONbits.TCKPS = 0b01; // 1:8 prescaler
        }
        else if(cycle_case == 2){
            T1CONbits.TCKPS = 0b10; // 1:64 prescaler
        }
        else{
            T1CONbits.TCKPS = 0b11; // 1:256 prescaler
        }
        PR1 = cycles;
        T1CONbits.TON = 1;
    }
}

// Function to setup and expire a timer with provided ms (max 200)
void tmr_wait_ms(int timer, int ms){
    tmr_setup_period(timer, ms);
    if (timer == TIMER1) {
        while(IFS0bits.T1IF == 0);
        T1CONbits.TON = 0;
        IFS0bits.T1IF = 0;
    } else if (timer == TIMER2) {
        while(IFS0bits.T2IF == 0);
        T2CONbits.TON = 0;
        IFS0bits.T2IF = 0;
    }
}