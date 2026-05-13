#include "xc.h"
#include <stdio.h>
#include <math.h>
#include "functions6.h"

char tx_array[SIZETX];
volatile CircularBuffer tx_buffer;
volatile int AN5_value;
volatile int AN11_value;
volatile int analog_pins_flag = 0;
volatile int send_to_uart_flag = 0;

void UART_configuration(){
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
    
    // Tx interrupt set up
    IFS0bits.U1TXIF = 0; // clearing Tx interrupt flag
    IPC3bits.U1TXIP = 1; // priority for Tx
    U1STAbits.UTXISEL0 = 0; // Tx interrupt triggers if a char is sent to U1TXREG,
    U1STAbits.UTXISEL1 = 0; // meaning U1TXREG has at least one slot empty ->
    // it will trigger right after enabling U1TX
    
    U1MODEbits.UARTEN = 1; // enable UART1
    U1STAbits.UTXEN = 1; // enable U1TX (transmission)
}

void part_1_assignment(){
    
    // ---* Configure UART *--- //
    UART_configuration();
    
    // ---* Configure ADC *--- //
    
    // - using ADC module 1 (registers 'AD1...')
    // - AVdd = Vrefh = 3.6V (considering 3.3V ?) / Vinl = AVss = Vrefl = 0v -> page 558
    AD1CON1bits.AD12B = 0; // 10-bit ADC mode (4 channels available)
    AD1CON3bits.ADRC = 0; // using internal clock cycle (Tcy) for ADC module
    
    // Tad definition: Tad = (ADCS + 1) * Tcy, Tad must be > 75ns for 10-bit ADC,
    // since Fcy = 72MHz -> Tcy = 13.89ns
    AD1CON3bits.ADCS = 8; // Tad = 9 * 13.89 = 125ns
    AD1CON1bits.ASAM = 0; // manual sampling
    AD1CON1bits.SSRC = 0; // manual conversion
    AD1CON2bits.CHPS = 0; // activating CH0 (1 channel mode -> CH0 needed to access AN11)
    AD1CON1bits.FORM = 0; // output format: unsigned integer (10-bit -> 0-1023)
    
    AD1CHS0bits.CH0SA = 11; // selecting AN11 for CH0 (linked to BAT-SENSE line for Vbat)
    AD1CHS0bits.CH0NA = 0; // selecting Vrefh for CH0
    
    ANSELBbits.ANSB11 = 1; // activating AN11
    TRISBbits.TRISB11 = 1;
    
    AD1CON1bits.ADON = 1; // activating ADC module 1
    
    buffer_init(&tx_buffer, tx_array, SIZETX); // Initialize the circular buffer

    long value = 0;
    long Vbattery_mV = 0;
    int cycle_counter = 0;
    int ret;
    
    tmr_setup_period(TIMER2,100);
    
    while(1){      
        if(cycle_counter == 5){
            // roughly executed every 500ms
            AD1CON1bits.DONE = 0; // ensuring done bit is zero before sampling
            AD1CON1bits.SAMP = 1; // starting sampling
            tmr_wait_ms(TIMER1,1); // wait sampling time (few micro seconds)
            AD1CON1bits.SAMP = 0; // starting conversion
            while(AD1CON1bits.DONE == 0); // waiting for conversion to finish
            value = ADC1BUF0; // reading result (0-1023)

            // converting to correct value:
            // Vbattery = (value/1023)*Vrefh*k (Volts) (where k is coefficient from voltage divider)
            // Vbattery_mV = (value/1023)*Vrefh_mV*k; (milliVolts)
            Vbattery_mV = (value * 3600L * 3L) / 1023L;
            cycle_counter = 0;
            send_battery_to_uart(Vbattery_mV);
        }
        cycle_counter++;
        ret = tmr_wait_period(TIMER2);
    }
}

void part_2_assignment(){
    // ---* Configure UART *--- //
    UART_configuration();
    
    // ---* Configure ADC *--- //
    
    // - using ADC module 1 (registers 'AD1...')
    // - AVdd = Vrefh = 3.6V (considering 3.3V ?) / Vinl = AVss = Vrefl = 0v -> page 558
    AD1CON1bits.AD12B = 0; // 10-bit ADC mode (4 channels available)
    AD1CON3bits.ADRC = 0; // using internal clock cycle (Tcy) for ADC module
    
    // Tad definition: Tad = (ADCS + 1) * Tcy, Tad must be > 75ns for 10-bit ADC,
    // since Fcy = 72MHz -> Tcy = 13.89ns
    AD1CON3bits.ADCS = 8; // Tad = 9 * 13.89 = 125ns
    AD1CON1bits.ASAM = 0; // manual sampling
    AD1CON3bits.SAMC = 16; // sample time ends after 16 Tad (2 mus)
    AD1CON1bits.SSRC = 7; // automatic conversion (when sampling stops)
    AD1CON2bits.CHPS = 0; // activating CH0 (1 channel mode -> CH0 needed to access AN11)
    AD1CON1bits.FORM = 0; // output format: unsigned integer (10-bit -> 0-1023)
    
    AD1CHS0bits.CH0SA = 5; // selecting AN5 for CH0 (pin OUT of IR Sensor board)
    AD1CHS0bits.CH0NA = 0; // selecting Vrefh for CH0
    
    ANSELBbits.ANSB5 = 1; // activating AN5
    TRISBbits.TRISB5 = 1;
    
    TRISBbits.TRISB4 = 0; // pin to enable IR Sensor board
    LATBbits.LATB4 = 1;
    
    AD1CON1bits.ADON = 1; // activating ADC module 1
    
    buffer_init(&tx_buffer, tx_array, SIZETX); // Initialize the circular buffer

    int value = 0;
    double Vsensor = 0.0; // volts
    double distance = 0.0; // meters
    int cycle_counter = 0;
    
    while(1){
        AD1CON1bits.DONE = 0; // ensuring done bit is zero before sampling
        AD1CON1bits.SAMP = 1; // starting sampling
        // sampling starts, and, after 16 Tad, automatically stops
        while(AD1CON1bits.DONE == 0); // waiting for conversion to finish
        value = ADC1BUF0; // reading result (0-1023)
        
        // converting to correct value:
        // Vsensor = (value/1023)*Vrefh (Volts)
        Vsensor = ((double)value * 3.6) / 1023.0;
        distance = 2.34 - 4.74*Vsensor + 4.06*pow(Vsensor,2) - 1.6*pow(Vsensor,3) + 0.24*pow(Vsensor,4);
        cycle_counter++;
        
        if(cycle_counter == 500){
            // roughly executed every 500ms
            cycle_counter = 0;
            send_sensor_distance_to_uart(distance);
        }
    }

}

void part_3_assignment(){
    // ---* Configure UART *--- //
    UART_configuration();
    
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
    
    ANSELBbits.ANSB11 = 1; // activating AN11
    TRISBbits.TRISB11 = 1;
    
    ANSELBbits.ANSB5 = 1; // activating AN5
    TRISBbits.TRISB5 = 1;
    
    TRISBbits.TRISB4 = 0; // pin to enable IR Sensor board
    LATBbits.LATB4 = 1;
    
    AD1CON1bits.ADON = 1; // activating ADC module 1
    
    buffer_init(&tx_buffer, tx_array, SIZETX); // Initialise the circular buffer
    tmr_setup_period(TIMER2,100); // 10Hz to send data to UART
    tmr_setup_period(TIMER3,1); // 1kHz to read data from AN5 and AN11

    double Vbattery = 0.0; // volts
    double Vsensor = 0.0;
    double distance_cm = 0.0; // centimetres
    int n_samples = 0;
    
    while(1){
        if(analog_pins_flag == 1){
            // frequency of 1kHz
            analog_pins_flag = 0;
            
            IEC0bits.AD1IE = 0; // disabling ADC interrupt (shared variables)
            // converting AN11 data to correct value (Volts):
            // Vbattery = (AN11_value/1023)*Vrefh*k (Volts) (where k is coefficient from voltage divider)
            Vbattery += ((double)AN11_value * 3.6 * 3.0) / 1023.0; // cumulative variable to compute average
            
            // converting AN5 data to correct value (centimetres):
            // Vsensor = (AN5_value/1023)*Vrefh (Volts)
            Vsensor = ((double)AN5_value * 3.6) / 1023.0;
            IEC0bits.AD1IE = 1; // enabling ADC interrupt again
            
            // applying formula to get distance (meters) registered by sensor
            // (not using pow() to increase computation speed)
            double Vsensor2 = Vsensor*Vsensor;
            double Vsensor3 = Vsensor2*Vsensor;
            double Vsensor4 = Vsensor3*Vsensor;
            //double distance = 2.34 - 4.74*Vsensor + 4.06*pow(Vsensor,2) - 1.6*pow(Vsensor,3) + 0.24*pow(Vsensor,4);
            double distance = 2.34 - 4.74*Vsensor + 4.06*Vsensor2 - 1.6*Vsensor3 + 0.24*Vsensor4;
            distance_cm += distance*100.0; // cumulative variable to compute average
            
            n_samples++;
        }
        
        if(send_to_uart_flag == 1){
            // frequency of 10Hz
            send_to_uart_flag = 0;
            
            // computing average values for AN5 and AN11 data
            Vbattery /= (double)n_samples;
            distance_cm /= (double)n_samples;
            
            send_values_to_uart(Vbattery,distance_cm);
            
            Vbattery = 0.0;
            distance_cm = 0.0;
            n_samples = 0;
        }
    }
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
        TMR2 = 0; // reset timer 2
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
        
        IFS0bits.T2IF = 0; // clear interrupt flag
        IPC1bits.T2IP = 0x01; // set priority level for timer 2
        IEC0bits.T2IE = 1; // enable interrupt
        
        T2CONbits.TON = 1;
        return;
    }
    else if(timer == TIMER3){
        T3CONbits.TCS = 0;
        T3CONbits.TGATE = 0;
        T3CONbits.TON = 0; // ensure timer is off
        TMR3 = 0; // reset timer 2
        IFS0bits.T3IF = 0; // clear state period flag before restarting
        if(cycle_case == 0){
            T3CONbits.TCKPS = 0b00; // 1:1 prescaler
        }
        else if(cycle_case == 1){
            T3CONbits.TCKPS = 0b01; // 1:8 prescaler
        }
        else if(cycle_case == 2){
            T3CONbits.TCKPS = 0b10; // 1:64 prescaler
        }
        else{
            T3CONbits.TCKPS = 0b11; // 1:256 prescaler
        }
        PR3 = cycles;
        
        IFS0bits.T3IF = 0; // clear interrupt flag
        IPC2bits.T3IP = 0x01; // set priority level for timer 3
        IEC0bits.T3IE = 1; // enable interrupt
        
        T3CONbits.TON = 1;
        return;
    }
}

void tmr_wait_ms(int timer, int ms){
    tmr_setup_period(timer, ms);
    if (timer == TIMER1) {
        while(IFS0bits.T1IF == 0);
        T1CONbits.TON = 0;
        IFS0bits.T1IF = 0;
    }
}

int tmr_wait_period(int timer){
    int temp = 0;
    if(timer == TIMER2){
        if(IFS0bits.T2IF == 1){
            temp = 1;
        }
        while(1){
            if(IFS0bits.T2IF == 1){
                // flag set
                IFS0bits.T2IF = 0; // flag cleared
                break;
            }
        }
    }
    return temp;
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

void send_battery_to_uart(long Vbattery_mV){
    char msg[24] = "";
    sprintf(msg,"$VBAT,%ld", Vbattery_mV);
    for(int i = 0;i<24;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt -> triggers interrupt
}

void send_sensor_distance_to_uart(double distance){
    char msg[24] = "";
    sprintf(msg,"$DIST,%f*", distance);
    for(int i = 0;i<24;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt -> triggers interrupt
}

void send_values_to_uart(double Vbattery, double distance_cm){
    char msg[30] = "";
    sprintf(msg,"$SENSE,%f,%f*", distance_cm, Vbattery);
    for(int i = 0;i<30;i++){
        if(msg[i] == '\0'){
            break;
        }
        buffer_write(&tx_buffer,msg[i]);
    }
    
    IEC0bits.U1TXIE = 1; // enabling Tx interrupt -> triggers interrupt
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

// ISR redefinition for ADC module 1
void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void){
    IFS0bits.AD1IF = 0;

    AN5_value = ADC1BUF0;
    AN11_value = ADC1BUF1;
    
    analog_pins_flag = 1; // read values of AN5 and AN11 in main at 1kHz
}

// Redefinition of ISR for timer 2
void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(void){
    IFS0bits.T2IF = 0; // clearing timer flag
    
    send_to_uart_flag = 1; // send values read from AN5 and AN11 to UART at 10Hz
}

// Redefinition of ISR for timer 3
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void){
    IFS0bits.T3IF = 0; // clearing timer flag
}