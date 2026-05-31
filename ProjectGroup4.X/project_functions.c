#include "xc.h"
#include "project_functions.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

char rx_array[SIZERX];
char tx_array[SIZETX];
volatile CircularBuffer rx_buffer;
volatile CircularBuffer tx_buffer;
volatile int button_E8_pressed;
volatile int button_E9_pressed;
volatile int AN11_value;
volatile int AN14_value;

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
    
    // ----* Remap Buttons ----* //
    RPINR0bits.INT1R = 88; // external interrupt 1 is mapped to button T2 (RP88)
    RPINR1bits.INT2R = 89; // external interrupt 2 is mapped to button T3 (RP89)
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt 1
    IFS1bits.INT2IF = 0; // clearing flag of external interrupt 2
    IPC5bits.INT1IP = 1; // priority level 1
    IPC7bits.INT2IP = 1; // priority level 1
    INTCON2bits.INT1EP = 1; // triggers on falling edge (1->0)
    INTCON2bits.INT2EP = 1; // triggers on falling edge (1->0)
    IEC1bits.INT1IE = 1; // enabling external interrupt 1
    IEC1bits.INT2IE = 1; // enabling external interrupt 2
    
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
    AD1CON1bits.ASAM = 0; // manual sampling
    AD1CON3bits.SAMC = 16; // sample time ends after 16 Tad (2 mus)
    AD1CON1bits.SSRC = 7; // automatic conversion (when sampling stops)
    AD1CON2bits.CHPS = 0; // activating CH0 (1 channel mode -> CH0 needed to access alternatively AN11 and AN14)
    AD1CON1bits.FORM = 0; // output format: unsigned integer (10-bit -> 0-1023)
    
    // Channel will be set accordingly when values will be read
    AD1CHS0bits.CH0NA = 0; // selecting Vrefh for CH0
        
    ANSELBbits.ANSB11 = 1; // activating AN11 for battery sensing
    TRISBbits.TRISB11 = 1;
    
    ANSELBbits.ANSB14 = 1; // activating AN14 for IR sensor voltage (buggy micro BUS)
    TRISBbits.TRISB14 = 1;
    
    TRISBbits.TRISB9 = 0; // pin to enable IR Sensor board (Buggy Mikrobus 1)
    LATBbits.LATB9 = 1; // enabled
    
    AD1CON1bits.ADON = 1; // activating ADC module 1

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
    
    // --- Prepare magnetometer --- //
    // Switching magnetometer to Sleep mode //
    tmr_wait_ms(TIMER1, 3); // wait 3ms to reach Suspended mode
    unsigned int adr = 0x4B & 0x7F;
    LATDbits.LATD6 = 0;
    spi_write(adr);
    spi_write(0x01);
    LATDbits.LATD6 = 1;
    
    // Switching magnetometer to Active mode //
    tmr_wait_ms(TIMER1, 3); // wait 3ms to reach Sleep mode
    adr = 0x4C & 0x7F;
    LATDbits.LATD6 = 0;
    spi_write(adr);
    spi_write(0x00);
    LATDbits.LATD6 = 1;
}

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
    else if(timer == TIMER2){
        T2CONbits.TCS = 0;
        T2CONbits.TGATE = 0;
        T2CONbits.TON = 0; // ensure timer is off
        TMR2 = 0; // reset timer 1
        IFS0bits.T2IF = 0; // clear state period flag before restarting
        if(cycle_case == 0){
            T2CONbits.TCKPS = 0b00; // 1:1 prescaler
        }
        else if(cycle_case == 1){
            T2CONbits.TCKPS = 0b01; // 1:8 prescaler
        }
        else if(cycle_case == 2){
            T2CONbits.TCKPS = 0b10; // 1:64 prescaler
        }
        else{
            T2CONbits.TCKPS = 0b11; // 1:256 prescaler
        }
        PR2 = cycles;
        T2CONbits.TON = 1;
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

// Function to setup and expire a timer with provided ms (max 200)
void tmr_wait_ms(int timer, int ms){
    tmr_setup_period(timer, ms);
    if (timer == TIMER2) {
        while(IFS0bits.T2IF == 0);
        T2CONbits.TON = 0;
        IFS0bits.T2IF = 0;
    }
}

void PWM_set(int speed, int yaw){
    
    int left_pwm = speed - yaw;
    int right_pwm = speed + yaw;
    int period = 7200;
    
    // saturates values up to +-100
    if(left_pwm >= 100){
        left_pwm = 100;
    }
    
    if(left_pwm <= -100){
        left_pwm = -100;
    }
    
    if(right_pwm >= 100){
        right_pwm = 100;
    }
    
    if(right_pwm <= -100){
        right_pwm = -100;
    }
    
    int left_DC = (left_pwm * period) / 100;
    int right_DC = (right_pwm * period) / 100;
    
    if (left_DC >= 0){
        if (right_DC >= 0){
            // left and right wheels forward
            DC_assigning(0,left_DC,0,right_DC);
        }
        else{
            // left wheels forward and right wheels backward
            DC_assigning(0,left_DC,-right_DC,0);
        }
    }
    else{
        if (right_DC >= 0){
            // right wheels forward and left wheels backward
            DC_assigning(-left_DC,0,0,right_DC);
        }
        else{
            // left and right wheels backward
            DC_assigning(-left_DC,0,-right_DC,0);
        } 
    }
}

void DC_assigning(int DC1, int DC2, int DC3, int DC4){
        
    OC1R = DC1; // PWM duty cycle
    OC1RS = 7200; // PWM period 10kHz
    
    OC2R = DC2; // PWM duty cycle
    OC2RS = 7200; // PWM period 10kHz
    
    OC3R = DC3; // PWM duty cycle
    OC3RS = 7200; // PWM period 10kHz
    
    OC4R = DC4; // PWM duty cycle
    OC4RS = 7200; // PWM period 10kHz
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

int parse_byte(parser_state* ps, char byte) {
    switch (ps->state) {
        case STATE_DOLLAR:
            if (byte == '$') {
                ps->state = STATE_TYPE;
                ps->index_type = 0;
            }
            break;
        case STATE_TYPE:
            if (byte == ',') {
                ps->state = STATE_PAYLOAD;
                ps->msg_type[ps->index_type] = '\0';
                ps->index_payload = 0; // initialise properly the index
            } else if (ps->index_type == 5) { // error! (type is PCREF for messages received)
                ps->state = STATE_DOLLAR;
                ps->index_type = 0;
			} else if (byte == '*') {
				ps->state = STATE_DOLLAR; // get ready for a new message
                ps->msg_type[ps->index_type] = '\0';
				ps->msg_payload[0] = '\0'; // no payload
                return NEW_MESSAGE;
            } else {
                ps->msg_type[ps->index_type] = byte; // ok!
                ps->index_type++; // increment for the next time;
            }
            break;
        case STATE_PAYLOAD:
            if (byte == '*') {
                ps->state = STATE_DOLLAR; // get ready for a new message
                ps->msg_payload[ps->index_payload] = '\0';
                return NEW_MESSAGE;
                } else if (ps->index_payload == 11) { // error (surpassed: two values + one comma + *)
                ps->state = STATE_DOLLAR;
                ps->index_payload = 0;
            } else {
                ps->msg_payload[ps->index_payload] = byte; // ok!
                ps->index_payload++; // increment for the next time;
            }
            break;
    }
    return NO_MESSAGE;
}

int extract_integer(const char* str) {
	int i = 0, number = 0, sign = 1;
	
	if (str[i] == '-') {
		sign = -1;
		i++;
	}
	else if (str[i] == '+') {
		sign = 1;
		i++;
	}
	while (str[i] != ',' && str[i] != '\0') {
		number *= 10; // multiply the current number by 10;
		number += str[i] - '0'; // converting character to decimal number
		i++;
	}
	return sign*number;
}		

int next_value(const char* msg, int i) {
	while (msg[i] != ',' && msg[i] != '\0') { 
        i++;
    }
	if (msg[i] == ','){
        i++;
    }
	return i;
}

// Parses one message (assumed correct structurer is $PCREF,speed,yaw*)
void task_read_speed_yaw(void* param){
    // frequency of 500Hz
    control_data *cd = (control_data*) param;
    parser_state ps;
    ps.state = STATE_DOLLAR;
    
    char byte;
    int res = NO_MESSAGE, counter = 0;
    while(buffer_is_empty(&rx_buffer) == 0 && counter <= 18){
        // keeps going until either circular buffer is empty or ps arrays are full
        buffer_read(&rx_buffer,&byte);
        if(parse_byte(&ps,byte) == NEW_MESSAGE){
            // takes only one message at a time
            res = NEW_MESSAGE;
            break;
        }
        counter++;
    }
    
    if(res != NEW_MESSAGE){
        // wrong message -> speed and yaw not updated
        return;
    }
    
    if(!strcmp(ps.msg_type, "PCREF") && ps.index_payload >= 3){
        // correct protocol and payload is not empty (smallest size is 3) (index_payload points at \0)
        
        // extracting values //
        counter = 0; // to know which value is being read
        int i = 0, j = 0;
        char temp[5]; // to store string value (dim = maximum value length + \0)
        while(i < ps.index_payload && counter < 2){
            j = next_value(ps.msg_payload,i);
            switch(counter){
                case 0:             
                    strncpy(temp, ps.msg_payload + i, j-1); // copy substring from index i of payload with length j-1
                    temp[j-1] = '\0';
                    cd->speed = extract_integer(temp);
                    break;
                case 1:
                    strncpy(temp, ps.msg_payload + i, j-i); // copy substring from index i of payload with length j-i
                    temp[j-i] = '\0';
                    cd->yaw = extract_integer(temp);
                    break;
            }
            i = j; // pointing next value
            counter++;
        }
    }
}

void task_PWM_set(void* param){
    // frequency of 500Hz
    control_data *cd = (control_data*) param;
    switch(cd->robot_state){
        case HALTED_STATE:
            DC_assigning(0, 0, 0, 0);
            break;
        case MOVING_STATE:
            PWM_set(cd->speed,cd->yaw);
            break;
        case OBSTACLE_AVOIDANCE_STATE:
            
            switch(cd->robot_sub_state){
                case AVOIDANCE_STEP_1:
                    // rotate 90° clockwise (later add gyroscope)
                    PWM_set(50,-50); // left_pwm = 100, right_pwm = 0 -> sharp rotation to the right
                    break;
                    
                // checking through gyroscope if in position
                    
                case AVOIDANCE_STEP_2:
                    // moving forward for two seconds (setting some flag to stop motion afterwards)
                    PWM_set(70,0); // left_pwm = 70, right_pwm = 70
                    break;
                case AVOIDANCE_STEP_3:
                    // rotate 90° anti-clockwise (later add gyroscope)
                    PWM_set(50,50); // left_pwm = 0, right_pwm = 100 -> sharp rotation to the left
                    break;
            }
            break;
    }
}

void task_button_check(void* param){
    // frequency of 10Hz (not required)
    control_data *cd = (control_data*) param;
    if(button_E8_pressed == 1){
        if(cd->robot_state == HALTED_STATE){
            cd->robot_state = MOVING_STATE;
        }
        else{
            // either moving or avoiding obstacle
            cd->robot_state = HALTED_STATE;
        }
    }
    if(button_E9_pressed == 1){
        // send number of data available inside TX and RX
    }
}

void task_reading_VBAT_n_sending_to_uart(){
    // frequency of 1Hz (to send, not considering separate frequency for reading since no requirements in project specs)
    double v_batt = 0.0;
    int AN11_value = 0;
    
    // manual sampling + auto conversion
    AD1CHS0bits.CH0SA = 11; // selecting AN11 for CH0 (pin OUT of battery voltage)
    AD1CON1bits.DONE = 0; // ensuring done bit is zero before sampling
    AD1CON1bits.SAMP = 1; // starting sampling
    // sampling starts, and, after 16 Tad, automatically stops
    while(AD1CON1bits.DONE == 0); // waiting for conversion to finish
    AN11_value = ADC1BUF0; // reading result (0-1023)
    // converting AN11 data to correct value (Volts):
    // Vbattery = (AN11_value/1023)*Vrefh*k (Volts) (where k is coefficient from voltage divider)
    v_batt = ((double)AN11_value * 3.6 * 3.0) / 1023.0; // cumulative variable to compute average
    
    // sending data to UART
    char msg[30] = "";
    sprintf(msg,"$MBATT,%.2f", v_batt);
    for(int i = 0;i<30;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt -> triggers interrupt
}

void task_reading_IR_value(void* param){
    // frequency of 500Hz
    control_data *cd = (control_data*) param;
    double Vsensor = 0.0;
    int AN14_value = 0;
    
    // manual sampling + auto conversion
    AD1CHS0bits.CH0SA = 14; // selecting AN14 for CH0 (pin OUT of IR sensor)
    AD1CON1bits.DONE = 0; // ensuring done bit is zero before sampling
    AD1CON1bits.SAMP = 1; // starting sampling
    // sampling starts, and, after 16 Tad, automatically stops
    while(AD1CON1bits.DONE == 0); // waiting for conversion to finish
    AN14_value = ADC1BUF0; // reading result (0-1023)
    // converting AN14 data to correct value (centimetres):
    // Vsensor = (AN14_value/1023)*Vrefh (Volts)
    Vsensor = ((double)AN14_value * 3.6) / 1023.0;

    // applying formula to get distance (meters) registered by sensor
    // (not using pow() to increase computation speed)
    double Vsensor2 = Vsensor*Vsensor;
    double Vsensor3 = Vsensor2*Vsensor;
    double Vsensor4 = Vsensor3*Vsensor;
    int distance = 2.34 - 4.74*Vsensor + 4.06*Vsensor2 - 1.6*Vsensor3 + 0.24*Vsensor4; // must be sent as an integer
    distance *= 100;
    cd->distance_sensor_value = distance;
    
    if(distance <= 20 && cd->robot_state == MOVING_STATE){
        // obstacle closer than 20 centimetres
        cd->robot_state = OBSTACLE_AVOIDANCE_STATE;
        cd->robot_sub_state = AVOIDANCE_STEP_1; // first phase (rotating of 90°)
    }
}

void task_sending_IR_value_to_uart(void* param){
    // frequency of 10Hz
    control_data *cd = (control_data*) param;
    
    // sending data to UART
    char msg[30] = "";
    sprintf(msg,"$MDIST,%d", cd->distance_sensor_value);
    for(int i = 0;i<30;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt -> triggers interrupt
}

void task_buggy_lights(void* param){
    // frequency of 1Hz
    control_data *cd = (control_data*) param;
    switch(cd->robot_state){
        case HALTED_STATE:
            LATBbits.LATB8 = !LATBbits.LATB8; // toggle left-side lights
            LATFbits.LATF1 = !LATFbits.LATF1; // toggle right-side lights
            LATGbits.LATG1 = 0; // low-lights off
            break;
        case MOVING_STATE:
            LATBbits.LATB8 = 0; // left-side lights off
            LATFbits.LATF1 = 0; // right-side lights off
            LATGbits.LATG1 = 1; // low-lights on
            break;
        case OBSTACLE_AVOIDANCE_STATE:
            LATFbits.LATF1 = !LATFbits.LATF1; // toggle right-side lights
            LATBbits.LATB8 = 0; // left-side lights off
            LATGbits.LATG1 = 1; // low-lights on
            break;
    };
}

void task_reading_magn_acc_gyro_n_sending_to_uart(void* param){
    // frequency of 10Hz (both reading and sending values)
    control_data *cd = (control_data*) param;
    
    // Reading x,y,z accelerometer values //
    int values[3];
    unsigned int output_1;
    unsigned int output_2;
    LATBbits.LATB3 = 0;
    spi_write(0x02 | 0x80); // providing first address (MSB x-axis)
    for(int i = 0;i<3;i++){
        output_1 = spi_write(0x00);
        output_2 = spi_write(0x00);
        output_1 = output_1 & 0x00F0; // clear last 4 bits
        output_2 = output_2 << 8; // left shift of 8 bits
        int data = output_2 | output_1;
        data = data / 16; // right shift of 16 bits
        values[i] = data;
    }
    LATBbits.LATB3 = 1;
    
    // Computing roll and pitch //
    cd->angle_values[0] = (int)(atan2(values[1], values[2]) * (180.0 / 3.14));
    cd->angle_values[1] = (int)(atan2(-values[0], sqrt((long)values[1] * (long)values[1] + (long)values[2] * (long)values[2])) * (180.0 / 3.14));
    
    // Reading x,y,z magnetometer values //
    LATDbits.LATD6 = 0;
    spi_write(0x42 | 0x80); // providing first address (MSB x-axis)
    for(int i = 0;i<3;i++){
        output_1 = spi_write(0x00);
        output_2 = spi_write(0x00);
        
        if(i<2){
            output_1 = output_1 & 0x00F8; // clearing last 3 bits of byte with 5 MSBs the 5 LSBs of x-axis
            output_2 = output_2 << 8; // shifting of 8 slots the 8 MSBs of x-axis
            int data = output_2 | output_1; // or between two parts (8 MSBs and 5 LSBs + 3 zero bits)
            data = data / 8; // safe shifting 13 bits (x-axis) to the left by 3 slots (13 bits in 16 bit register)
            values[i] = data;
        }
        else{
            // z-axis is allocated in a different way
            output_1 = output_1 & 0x00FE; // clear bit 0, keep bits [7:1]
            output_2 = output_2 << 8; // move MSBs to upper byte
            int data = output_2 | output_1; // combine parts
            data = data / 2; // shift right by 1
            values[i] = data;
        }
    }
    LATDbits.LATD6 = 1;
    
    // Computing yaw with magnetometer values //
    // magnetic field of motor wheels may disturb it
    cd->angle_values[2] = (int)(atan2(values[1], values[0]) * 180.0 / 3.14);
    
    // Reading z gyroscope value //
    // useful to compute yaw
    LATBbits.LATB4 = 0;
    spi_write(0x06 | 0x80); // providing first address (MSB x-axis)
    output_1 = spi_write(0x00);
    output_2 = spi_write(0x00);
    output_2 = output_2 << 8; // left shift of 8 bits (MSB)
    int16_t raw_gz = output_2 | output_1; 
    // 16 bit value (need a 16-bit integer to ensure sign is kept and not converted in a grater value once inside a 32-bit integer)
    LATBbits.LATB4 = 1;
    
    // Computing yaw with gyroscope value //
    // need to integrate the value since it is an angular velocity component
    float gz_dps = ((float)raw_gz * 2000.0f) / 32767.0f; // converting value from [32767;-32767] to [2000;-2000] scale
    float dt = 0.1f; // 100ms -> value read at 10Hz
    cd->yaw_ctrl += gz_dps * dt; // value used to control car movement
    
    // Value retrieved is a rotation obtained from z component of angular velocity, so it could grow beyond certain limits.
    // Ensure integration doesn't exit range [-180;180] (keep consistency with yaw from magnetometer):
    if(cd->yaw_ctrl > 180.0f){
        cd->yaw_ctrl -= 360.0f;
    }
    if(cd->yaw_ctrl <= -180.0f){
        cd->yaw_ctrl += 360.0f;
    }
    
    /*
     gz may introduce drift -> progressive cumulative bias,
     in that case we can combine with yaw from magnetometer:
     final_yaw = 0.98f * (final_yaw + gz_dps * dt) + 0.02f * yaw_mag;
     
     using this instead of previous formula: float yaw += gz_dps * dt;
     */
}

void scheduler(heartbeat schedInfo[], int nTasks){
    int i;
    for (i = 0; i < nTasks; i++) {
        schedInfo[i].n++;
        if (schedInfo[i].enable == 1 && schedInfo[i].n >= schedInfo[i].N) {
            schedInfo[i].f(schedInfo[i].params);            
            schedInfo[i].n = 0;
        }
    }
}

// ISR redefinition for UART1 Tx register
void __attribute__((__interrupt__, __auto_psv__)) _U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0;
    
    // Reading all characters in the circular buffer buffer (until empty),
    // and writing them in the Tx buffer until it is full
    while(U1STAbits.UTXBF == 0 && !buffer_is_empty(&tx_buffer)){
        // - checking if there is anything to send, since the interrupt will trigger right after enabling U1TX
        // - Tx buffer is not full, at least one more char can be written
            char c;
            buffer_read(&tx_buffer, &c);
            U1TXREG = c;
    }
    
    if(buffer_is_empty(&tx_buffer)){
        // Tx buffer could be full or not, in any case no more data to send
        IEC0bits.U1TXIE = 0; // disable interrupt to avoid multiple triggers
    }
    
    // In case the circular buffer is not empty, the enabler remains set, 
    // so, as soon as one byte is sent, thus freeing on slot in the Tx buffer,
    // the interrupt will trigger again
}

// ISR redefinition for UART1 Rx register
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0;
    
    // Reading all characters in the Rx buffer (until empty) and writing them in the circular buffer
    while(U1STAbits.URXDA == 1){
        buffer_write(&rx_buffer, U1RXREG);
    }
}

// Redefinition of ISR for external interrupt 1 (robust implementation)
void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt(void){
    IFS1bits.INT1IF = 0; // clearing flag of external interrupt
    IPC2bits.T3IP = 0x01; // set priority to one
    IEC0bits.T3IE = 1;
    tmr_setup_period(TIMER3,10);
}

// Redefinition of ISR for external interrupt 2
void __attribute__((__interrupt__, __auto_psv__)) _INT2Interrupt(void){
    IFS1bits.INT2IF = 0; // clearing flag of external interrupt
    IPC6bits.T4IP = 0x01; // set priority to one
    IEC1bits.T4IE = 1;
    tmr_setup_period(TIMER4,10);
}

// Redefinition of ISR for timer 3
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void){
    T3CONbits.TON = 0;
    IFS0bits.T3IF = 0; // clearing timer flag
    IEC0bits.T3IE = 0; // disabling timer interrupt
    
    if(PORTEbits.RE8 == 0){
        button_E8_pressed = 1;
    }
}

// Redefinition of ISR for timer 4
void __attribute__((__interrupt__, __auto_psv__)) _T4Interrupt(void){
    T4CONbits.TON = 0;
    IFS1bits.T4IF = 0; // clearing timer flag
    IEC1bits.T4IE = 0; // disabling timer interrupt
    
    if(PORTEbits.RE9 == 0){
        button_E9_pressed = 1;
    }
    
}