#include "state_machine.h"
#include "hal.h"
#include <stdio.h>

// Hardcoded Thresholds
#define COOLING_THRESHOLD   70.0f
#define OVERHEAT_THRESHOLD  90.0f

// Internal variable to track current state
static SystemState_e current_state = SystemState_STATE_OFF;

typedef struct {
    uint8_t pump_duty;
    uint8_t fan_duty;
} StateOutputs_t;

// Simplified "Lookup table": index corresponds to SystemState_e enum
static const StateOutputs_t state_outputs[] = {
    [SystemState_STATE_OFF]      = {0,   0},
    [SystemState_STATE_STANDBY]  = {10, 10},
    [SystemState_STATE_COOLING]  = {60, 60},
    [SystemState_STATE_OVERHEAT] = {100,100}
};

void SM_Init(void){
    current_state = SystemState_STATE_OFF;
    HAL_SetPump(state_outputs[current_state].pump_duty);
    HAL_SetFan(state_outputs[current_state].fan_duty);
}

/*  
    State machine diagram (written as an adjacency  list)
        OFF: STANDBY 
        STANDBY: OFF, COOLING, OVERHEAT 
        COOLING: STANDBY, OVERHEAT 
        OVERHEAT: COOLING, STANDBY
*/ 
void SM_Update(bool ignition, float temperature, bool print_transition){
    SystemState_e previous_state = current_state;

    // State Transition Logic
    switch (current_state) {
        
        case SystemState_STATE_OFF:
            if (ignition) current_state = SystemState_STATE_STANDBY;
            break;
        
        case SystemState_STATE_STANDBY:
            if (!ignition) {
                current_state = SystemState_STATE_OFF;
            } else if (temperature >= OVERHEAT_THRESHOLD) {
                current_state = SystemState_STATE_OVERHEAT;
            } else if (temperature >= COOLING_THRESHOLD) {
                current_state = SystemState_STATE_COOLING;
            }
            break;

        case SystemState_STATE_COOLING:
            if (temperature < COOLING_THRESHOLD) {
                current_state = SystemState_STATE_STANDBY;
            } else if (temperature >= OVERHEAT_THRESHOLD) {
                current_state = SystemState_STATE_OVERHEAT;
            }
            break;

        case SystemState_STATE_OVERHEAT:
            if (temperature < COOLING_THRESHOLD) {
                current_state = SystemState_STATE_STANDBY;
            } else if (temperature < OVERHEAT_THRESHOLD) {
                current_state = SystemState_STATE_COOLING;
            }
            break;

        default:
            current_state = SystemState_STATE_OFF;
            break;

    }

    // Update outputs based on current state
    HAL_SetPump(state_outputs[current_state].pump_duty);
    HAL_SetFan(state_outputs[current_state].fan_duty);

    // print state change
    if (print_transition && previous_state != current_state) {
        printf("[STATE CHANGE] %d -> %d\n", previous_state, current_state);
    }
}

SystemState_e SM_GetState(void) {
    return current_state;
}