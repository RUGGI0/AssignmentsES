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
    cd.angle_values[0] = 0;
    cd.angle_values[1] = 0;
    cd.angle_values[2] = 0;
    cd.one_time_exec = 0;
    
    ps->index_type = 0;
    ps->state = STATE_DOLLAR;
    
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
    schedInfo[2].f = task_stop_buggy_after_2sec;
    schedInfo[2].params = (void*)(&cd);
    schedInfo[2].enable = 0; // initially not active (activated when needed)
    
    // Task to check if either button has been clicked (10Hz)
    schedInfo[3].n = 0;
    schedInfo[3].N = 50;
    schedInfo[3].f = task_button_check;
    schedInfo[3].params = (void*)(&cd);
    schedInfo[3].enable = 1;
    
    // Task to read VBAT (1Hz)
    schedInfo[4].n = -144;
    schedInfo[4].N = 500;
    schedInfo[4].f = task_reading_VBAT_n_sending_to_uart;
    schedInfo[4].params = (void*)(&cd);
    schedInfo[4].enable = 1;
    
    // Task to read IR value from collision sensor (500Hz)
    // could be better to implement average to clear outlayers
    schedInfo[5].n = -1;
    schedInfo[5].N = 1;
    schedInfo[5].f = task_reading_IR_value;
    schedInfo[5].params = (void*)(&cd);
    schedInfo[5].enable = 1;
    
    // Task to send IR value to uart (10Hz)
    schedInfo[6].n = -12;
    schedInfo[6].N = 50;
    schedInfo[6].f = task_sending_IR_value_to_uart;
    schedInfo[6].params = (void*)(&cd);
    schedInfo[6].enable = 1;
    
    // Task to activate buggy lights (10Hz)
    schedInfo[7].n = -25;
    schedInfo[7].N = 50;
    schedInfo[7].f = task_buggy_lights;
    schedInfo[7].params = (void*)(&cd);
    schedInfo[7].enable = 1;
    
    // Task to receive IMU data and compute roll,pitch and yaw (10Hz)
    schedInfo[8].n = -37;
    schedInfo[8].N = 50;
    schedInfo[8].f = task_reading_magn_acc_gyro;
    schedInfo[8].params = (void*)(&cd);
    schedInfo[8].enable = 1;
    
    // Task to send angle value to uart (10Hz)
    schedInfo[9].n = -18;
    schedInfo[9].N = 250;
    schedInfo[9].f = sending_angle_values_to_uart;
    schedInfo[9].params = (void*)(&cd);
    schedInfo[9].enable = 1;
   
    //int cycle_counter = 0;
    
    while(1){
        // maybe with buggy lights
        //if(cycle_counter == 250){
            // LD0 blinks every 500ms (short task can be inserted in main loop)
            //LATAbits.LATA0 = !LATAbits.LATA0;
            //cycle_counter = 0;
        //}
        scheduler(schedInfo, MAX_TASKS);
        tmr_wait_period(TIMER1);
        //cycle_counter++;
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