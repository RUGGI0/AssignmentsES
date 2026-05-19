#include "xc.h"
#include "functions8.h"
#include <math.h>

char rx_array[SIZERX];
volatile CircularBuffer rx_buffer;
volatile heartbeat schedInfo[MAX_TASKS];

void buffer_init(volatile CircularBuffer* cb, char* array_ptr, int max_size) {
    cb->buffer = array_ptr; 
    cb->size = max_size;
    cb->head = 0;
    cb->tail = 0;
}

int buffer_is_empty(volatile CircularBuffer* cb) {
    return cb->head == cb->tail;
}

int buffer_write(volatile CircularBuffer* cb, char c) {
    int next = (cb->head + 1) % cb->size;
    if (next == cb->tail) {
        return -1; // Buffer is full
    }
    cb->buffer[cb->head] = c;
    cb->head = next;
    return 0;
}

int buffer_read(volatile CircularBuffer* cb, char* c) {
    if (buffer_is_empty(cb)) {
        return -1; // Buffer is empty
    }
    *c = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % cb->size;
    return 0;
}

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

int tmr_wait_period(int timer){
    int temp = 0;
    if(timer == TIMER1){
        if(IFS0bits.T1IF == 1){
            temp = 1;
        }
        while(1){
            if(IFS0bits.T1IF == 1){
                // flag set
                IFS0bits.T1IF = 0; // flag cleared
                break;
            }
        }
    }
    return temp;
}

void scheduler(){
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        schedInfo[i].n++;
        if (schedInfo[i].enable == 1 && schedInfo[i].n >= schedInfo[i].N) {
            schedInfo[i].f(schedInfo[i].params); 
            schedInfo[i].n = 0;
        }
    }
}

void task1_update_feedback(void* param){
    control_data *cd = (control_data*) param;
    char msg[15];
    int counter = 0;
    int tail = -1;
    while(buffer_is_empty(&rx_buffer) == 0){
        IEC0bits.U1RXIE = 0; 
        // disabling interrupt before using rx_buffer to ensure it is not accessed inside U1RXInterrupt at the same time
        buffer_read(&rx_buffer, &msg[counter]);
        IEC0bits.U1RXIE = 1;
        if(counter == 14 && msg[counter] != '*'){
            // in case invalid message is received (no ending *)
            cd->new_values = 0; // invalid values
            counter = 0;
            continue;
        }
        if(msg[counter] == '*'){
            // message is ready to be parsed
            msg[counter] = '_'; // avoids triggering if statement again at next loop
            
            tail = --counter; // second number (yaw)
            
            // checking first part of protocol
            if(msg[0] == '$' && msg[1] == 'R' && msg[2] == 'E' &&
                    msg[3] == 'F' && msg[4] == ','){
                // correct sequence of chars with initial $
                
                // yaw analysis //
                int value = 0;
                int ret = parse_value_from_uart(msg,&tail,&value);
                if(ret == 0 || msg[tail] != ',' || value > 100 || value < -100){
                    cd->new_values = 0; // invalid values
                    counter = -1;
                    continue;
                }
                cd->yaw = value;
                tail--; // jumping comma (pointing to last digit of speed)
                
                // speed analysis //
                ret = parse_value_from_uart(msg,&tail,&value);
                if(ret == 0 || msg[tail] != ',' || value > 100 || value < -100){
                    cd->new_values = 0; // invalid values
                    counter = -1;
                    continue;
                }
                cd->speed = value;
            }
            else{
                cd->new_values = 0;
                counter = -1;
                continue;
            }
            
            // there may be another message ready in rx_buffer
            counter = -1; // ready for next message (incremented outside if)
        }
        counter++;
    }
}

int parse_value_from_uart(char msg[15], int* tail, int* value){
    // take at max 3 numbers
    int temp = (*tail) - 3;
    int counter = 0;
    int multiplier = 1;
    (*value) = 0; // value reset
    for(int i = (*tail);i>=temp && i >= 0;i--){
        if(msg[i] == ','){
            // number finished
            (*tail) = i; // tail points to comma
            return counter;
        }
        if(msg[i] == '-'){
            // number finished with sign
            (*tail)--; // jumping sign (points to comma if correct msg)
            (*value) *= -1;
            return counter;
        }
        if(msg[i] < '0' || msg[i] > '9'){
            // invalid chars
            return 0;
        }
        (*tail)--;
        (*value) += (msg[i] - '0')*multiplier;
        multiplier *= 10;
        counter++; // number of digit viewed
    }
    return counter;
}

void task2_update_feedback(void* param){
    
}

void task3_update_feedback(void* param){
    
}

void task4_update_feedback(void* param){
    
}

// ISR redefinition for UART1 Rx register
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0;
    
    // Reading all characters in the Rx buffer (until empty) and writing them in the circular buffer
    while(U1STAbits.URXDA == 1){
        buffer_write(&rx_buffer, U1RXREG);
    }
}