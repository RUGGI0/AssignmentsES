#include "xc.h"
#include "project_functions.h"

int main(void) {
        
    // Peripherals configuration //
    device_init();
    
    // Variable initialisation //
    button_E8_pressed = 0;
    button_E9_pressed = 0;
    buffer_init(&rx_buffer, rx_array, SIZERX);
    buffer_init(&tx_buffer, tx_array, SIZETX);
    heartbeat schedInfo[MAX_TASKS];
    // scheduler configuration
    
    
    tmr_setup_period(TIMER1,2); // control loop executes at 500Hz (2ms)
            
    int cycle_counter = 0;
    
    while(1){
        if(cycle_counter == 250){
            // LD0 blinks every 500ms (short task can be inserted in main loop)
            LATAbits.LATA0 = !LATAbits.LATA0;
            cycle_counter = 0;
        }
        scheduler(schedInfo, MAX_TASKS);
        tmr_wait_period(TIMER1);
        cycle_counter++;
    }
    
    return 0;
}
