/*
 * File:   functions5.c
 * Author: gregd
 *
 * Created on April 12, 2026, 6:04 PM
 */


#include "xc.h"
#include <stdio.h>
#include "functions5.h"

void assignment_1(){
    
    unsigned int adr = 0;
    unsigned int data = 0;
    unsigned int output = 0;
    int r_w = -1; // 0 -> write, 1 -> read
    
    // --- Switching magnetometer to Sleep mode ---
    tmr_wait_ms(TIMER1, 5); // wait 5ms to reach Suspended mode (pag 21 - notes 7)
    adr = 0x4B; // register address used to change from Suspended mode to Sleep mode
    data = 0x01; // data to write (changing from Suspended mode to Sleep mode)
    r_w = 0; // writing
    output = auxiliary_spi_write(adr,data,r_w);
    
    // --- Switching magnetometer to Active mode ---
    tmr_wait_ms(TIMER1, 5); // wait 5ms to reach Sleep mode (pag 22 - notes 7) 
    adr = 0x4C; // register address used to change from Sleep mode to Active mode
    data = 0x00; // data to write (changing from Sleep mode to Active mode)
    r_w = 0; // writing
    output = auxiliary_spi_write(adr,data,r_w);
    
    // --- Reading Chip ID ---
    tmr_wait_ms(TIMER1, 5); // wait 5ms to reach Active mode (not sure) 
    adr = 0x40; // register address with Chip ID info
    data = 0x00; // dummy input to retrieve info (Chip ID)
    r_w = 1; // reading
    output = auxiliary_spi_write(adr,data,r_w);
    
    // --- Sending Chip ID to UART ---
    // need loop to correctly send information
    while(1){
        tmr_wait_ms(TIMER1, 150);
        char msg[SIZE] = "";
        sprintf(msg, "ID=%d",output);
        for(int i = 0;i<SIZE;i++){
            if(msg[i] == '\0'){
                break;
            }
            while(U1STAbits.UTXBF == 1); // Tx buffer is full -> wait
            U1TXREG = msg[i];
        }
    }
}

void assignment_2(){
    
    unsigned int adr = 0;
    unsigned int data = 0;
    unsigned int output_1 = 0;
    unsigned int output_2 = 0;
    int r_w = -1; // 0 -> write, 1 -> read
    char msg[SIZE] = "";
    
    // --- Switching magnetometer to Sleep mode ---
    tmr_wait_ms(TIMER1, 5); // wait 5ms to reach Suspended mode (pag 21 - notes 7)
    adr = 0x4B; // register address used to change from Suspended mode to Sleep mode
    data = 0x01; // data to write (changing from Suspended mode to Sleep mode)
    r_w = 0; // writing
    output_1 = auxiliary_spi_write(adr,data,r_w);
    
    // --- Switching magnetometer to Active mode ---
    tmr_wait_ms(TIMER1, 5); // wait 5ms to reach Sleep mode (pag 22 - notes 7) 
    adr = 0x4C; // register address used to change from Sleep mode to Active mode
    data = 0x00; // data to write (changing from Sleep mode to Active mode, and setting 10Hz frequency)
    r_w = 0; // writing
    output_1 = auxiliary_spi_write(adr,data,r_w);
    
    // --- Reading x-axis of magnetometer (13 bits) ---
    while(1){
        tmr_wait_ms(TIMER1, 5); // wait 5ms to reach Active mode (not sure)
        
        // -- Reading first portion (register 0x42) --
        adr = 0x42; // register address with 5 LSBs
        data = 0x00; // dummy input to retrieve info (Chip ID)
        r_w = 1; // reading
        output_1 = auxiliary_spi_write(adr,data,r_w); // 5 LSBs as MSBs in a byte
        
        // -- Reading second portion (register 0x43)
        adr = 0x43; // register address with 8 MSBs
        data = 0x00; // dummy input to retrieve info (Chip ID)
        r_w = 1; // reading
        output_2 = auxiliary_spi_write(adr,data,r_w);
        
        // alternative is to clock out 0x00 and automatically reads next register info

        output_1 = output_1 & 0x00F8; // clearing last 3 bits of byte with 5 MSBs the 5 LSBs of x-axis
        output_2 = output_2 << 8; // shifting of 8 slots the 8 MSBs of x-axis
        int res = output_2 | output_1; // or between two parts (8 MSBs and 5 LSBs + 3 zero bits)
        res = res / 8; // safe shifting 13 bits (x-axis) to the left by 3 slots (13 bits in 16 bit register)
        
        /*
        // alternative for message composing
        uint16_t lsb = (output_1 & 0xF8) >> 3;// clearing last 3 bits of byte with 5 MSBs the 5 LSBs of x-axis,
        // and shifting them of 3 slots
        uint16_t msb = output_2 << 5; // shifting 8 MSBs by 5 slots to make space for 5 LSBs

        uint16_t res = msb | lsb; // or between two parts (8 MSBs and 5 LSBs)

        // sign extend (x-axis value has sign)
        int16_t signed_res;

        if (res & 0x1000)   // bit 12 is sign bit
        {
            signed_res = res | 0xE000;  // extend sign
        }
        else
        {
            signed_res = res;
        }
         * */
        
        // --- Sending x-axis value of magnetometer to UART ---
        // need loop to correctly send information
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

unsigned int auxiliary_spi_write(unsigned int adr, unsigned int data, int r_w){
    unsigned int dummy = 0;
    unsigned int value = 0;
    if(r_w == 0){
        // write
        adr = adr & 0x7F;
    }
    else{
        // read
        adr = adr | 0x80;
    }
    LATDbits.LATD6 = 0; // magnetometer (on)
    dummy = spi_write(adr); // sending address to read/write
    value = spi_write(data); // sending data to write/clock out
    LATDbits.LATD6 = 1; // magnetometer (off)
    return value;
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