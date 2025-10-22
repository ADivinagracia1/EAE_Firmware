#include "state_machine.h"
#include "hal.h"
#include <stdio.h>

// Hardcoded Thresholds
#define COOLING_THRESHOLD   70.0f
#define OVERHEAT_THRESHOLD  90.0f

// Internal variable to track current state
static SystemState_t current_state = STATE_OFF;

typedef struct {
    uint8_t pump_duty;
    uint8_t fan_duty;
} StateOutputs_t;

// Simplified "Lookup table": index corresponds to SystemState_t enum
static const StateOutputs_t state_outputs[] = {
    [STATE_OFF]      = {0,   0},
    [STATE_STANDBY]  = {10, 10},
    [STATE_COOLING]  = {60, 60},
    [STATE_OVERHEAT] = {100,100}
};

void SM_Init(void){
    current_state = STATE_OFF;
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
    SystemState_t previous_state = current_state;

    // State Transition Logic
    switch (current_state) {
        
        case STATE_OFF:
            if (ignition) current_state = STATE_STANDBY;
            break;
        
        case STATE_STANDBY:
            if (!ignition) {
                current_state = STATE_OFF;
            } else if (temperature >= OVERHEAT_THRESHOLD) {
                current_state = STATE_OVERHEAT;
            } else if (temperature >= COOLING_THRESHOLD) {
                current_state = STATE_COOLING;
            }
            break;

        case STATE_COOLING:
            if (temperature < COOLING_THRESHOLD) {
                current_state = STATE_STANDBY;
            } else if (temperature >= OVERHEAT_THRESHOLD) {
                current_state = STATE_OVERHEAT;
            }
            break;

        case STATE_OVERHEAT:
            if (temperature < COOLING_THRESHOLD) {
                current_state = STATE_STANDBY;
            } else if (temperature < OVERHEAT_THRESHOLD) {
                current_state = STATE_COOLING;
            }
            break;

        default:
            current_state = STATE_OFF;
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

SystemState_t SM_GetState(void) {
    return current_state;
}