#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SystemState_STATE_OFF = 0,
    SystemState_STATE_STANDBY,
    SystemState_STATE_COOLING,
    SystemState_STATE_OVERHEAT
} SystemState_e;

// Initialize state machine
void SM_Init(void);

// Update state machine based on inputs
// temperature: current coolant temperature
// ignition: ignition state (true=on, false=off)
// print_transition: if true, prints a message when the state changes
void SM_Update(bool ignition, float temperature, bool print_transition);

SystemState_e SM_GetState(void);

#endif // STATE_MACHINE_H
