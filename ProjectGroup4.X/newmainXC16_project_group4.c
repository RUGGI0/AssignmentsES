#include "xc.h"
#include "project_functions.h"

int main(void) {
    
    // debug stuff
    TRISGbits.TRISG9 = 0;
    LATGbits.LATG9 = 0;
    
    // Peripherals configuration //
    device_init();
    
    // Variable initialisation //
    button_E8_pressed = 0;
    button_E9_pressed = 0;
    buffer_init(&rx_buffer, rx_array, SIZERX);
    buffer_init(&tx_buffer, tx_array, SIZETX);
    tmr_setup_period(TIMER1,2); // control loop executes at 500Hz (2ms)
    
    heartbeat schedInfo[MAX_TASKS];
    
    parser_state *ps;
    parser_state ps_value;
    ps = &ps_value;
    
    // Shared parameters between tasks //
    control_data cd;
    cd.speed = 0;
    cd.yaw = 0;
    cd.robot_state = HALTED_STATE;
    cd.robot_sub_state = AVOIDANCE_STEP_0;
    cd.gyro_yaw = 0.0f; // value of yaw to control steering (cumulative change with integration)
    cd.schedInfo = schedInfo;
    cd.par_state = ps;
    cd.obs_av_state_ctrl = 0;
    
    // Scheduler configuration //
    // Task to read speed and yaw coming from UART (500Hz)
    schedInfo[0].n = 0;
    schedInfo[0].N = 1;
    schedInfo[0].f = task_read_speed_yaw;
    schedInfo[0].params = (void*)(&cd);
    schedInfo[0].enable = 1;
    
    // Task to set PWM for buggy motion (500Hz)
    schedInfo[1].n = -1;
    schedInfo[1].N = 1;
    schedInfo[1].f = task_PWM_set;
    schedInfo[1].params = (void*)(&cd);
    schedInfo[1].enable = 1;
    
    // Task to stop 2sec buggy motion in obstacle avoidance mode
    schedInfo[2].N = 1000; // 2sec
    schedInfo[2].enable = 0; // initially not active (activated when needed)
    
    // Task to check if either button has been clicked (10Hz)
    schedInfo[3].n = 0;
    schedInfo[3].N = 50;
    schedInfo[3].f = task_button_check;
    schedInfo[3].params = (void*)(&cd);
    schedInfo[3].enable = 1;
    
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
        
        if(cd.speed == 80){
            LATGbits.LATG9 = 1;
        }
    }
    
    return 0;
}

// eventual final changes: combine tasks with same frequency

// Things to ask prof:
// - is there a frequency at which distance and v_batt are taken or just use ADC interrupt?
// - okay to print v_batt with two digits, not rounding nor truncating?
// - for magnetometer do we have to read all and only the axes of the magnetic field?

// Possible changes:
// - move state check logic from task_reading_IR_value and task_reading_magn_acc_gyro to task_PWM_set