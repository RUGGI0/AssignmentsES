#include "xc.h"
#include "project_functions.h"

int main(void) {
        
    // peripherals configuration
    device_init();
    
    // scheduler configuration
    heartbeat schedInfo[MAX_TASKS];
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
