#include "hal.h"
#include <stdio.h>
#include <stdint.h>

#define CAN_QUEUE_SIZE 16

typedef struct {
    uint32_t id;
    float value;
}CAN_Message;

// Incoming messages queue
static CAN_Message can_in_queue[CAN_QUEUE_SIZE];
static int can_in_head = 0;
static int can_in_tail = 0;

// Outgoing messages queue (simplest: just print)
static CAN_Message can_out_queue[CAN_QUEUE_SIZE];
static int can_out_head = 0;
static int can_out_tail = 0;

// ---------------------- CAN Functions ----------------------

void HAL_SendCANMessage(uint32_t id, float value) {
    // For simplicity, just print the message
    printf("[CAN TX] ID: 0x%X | Value: %.2f\n", id, value);
    // Optionally, store in out queue for debugging
}

bool HAL_ReadCANMessage(uint32_t *id, float *value) {
    if (can_in_head == can_in_tail) {
        // No messages
        return false;
    }
    *id = can_in_queue[can_in_head].id;
    *value = can_in_queue[can_in_head].value;
    can_in_head = (can_in_head + 1) % CAN_QUEUE_SIZE; // wrap around
    return true;
}

void HAL_GenerateCANMessage(uint32_t id, float value) {
    int next_tail = (can_in_tail + 1) % CAN_QUEUE_SIZE;
    if (next_tail == can_in_head) {
        // Queue full, discard oldest message
        can_in_head = (can_in_head + 1) % CAN_QUEUE_SIZE;
    }
    can_in_queue[can_in_tail].id = id;
    can_in_queue[can_in_tail].value = value;
    can_in_tail = next_tail;
}


// ---------------------- Hardware Abstractions ----------------------

typedef struct {
    bool ignition_switch;  // On or Off
    float temperature_c;   // Degrees C (note: clampeded to 20C to 90C for simulation)
    uint8_t pump_duty;     // 0-100% duty cycle
    uint8_t fan_duty;      // 0-100% duty cycle
} SimulatedHardware_t;

static SimulatedHardware_t hw = {
    .ignition_switch = false,
    .temperature_c = 25.0f,
    .pump_duty = 0,
    .fan_duty = 0
};


// === Hardware Access Functions ===
void HAL_Init(void) {
    hw.ignition_switch = false;
    hw.temperature_c = 25.0f;
    hw.pump_duty = 0;
    hw.fan_duty = 0;
}

/*
    NOTE: HAL_ReadIgnition and HAL_ReadTemperature dictate how the hardware is 
    simulated. Every 5 loops, the ignition switch will turn on and off. When the 
    system is on, temperature reading increases. Otherwise, temperature decreases.
    This way, the state changes in the state machine can be showcased.
*/

bool HAL_ReadIgnition(void) {
    // Simulate ignition switch read (toggles every 5 cycles for testing)
    static uint8_t counter = 0;
    counter++;
    if (counter % 5 == 0) {
        hw.ignition_switch = !hw.ignition_switch;
    }
    return hw.ignition_switch;
}

float HAL_ReadTemperature(void) {
    // simulate temperature increase if ignition ON, decrease otherwise
    if (hw.ignition_switch) hw.temperature_c += 0.5f;
    else hw.temperature_c -= 0.2f;

    // Clamp temperature within [20, 100]
    if (hw.temperature_c < 20.0f) hw.temperature_c = 20.0f;
    if (hw.temperature_c > 100.0f) hw.temperature_c = 100.0f;

    return hw.temperature_c;
}

void HAL_SetInitialTemperature(float temp){
    // Clamp temperature within [20, 100]
    if (temp < 20.0f) temp = 20.0f;
    if (temp > 100.0f) temp = 100.0f;
    hw.temperature_c = temp;
}

void HAL_SetPump(uint8_t duty_cycle){
    // Simulate pump control limited to 100% duty cycle
    if (duty_cycle > 100) duty_cycle = 100;

    if (duty_cycle != hw.pump_duty) {
        hw.pump_duty = duty_cycle;
        // printf(">> Pump duty cycle: %u%%\n", hw.pump_duty);
    }
}

void HAL_SetFan(uint8_t duty_cycle){
    // Simulate fan control limited to 100% duty cycle
    if (duty_cycle > 100) duty_cycle = 100;

    if (duty_cycle != hw.fan_duty) {
        hw.fan_duty = duty_cycle;
        // printf(">> Fan duty cycle: %u%%\n", hw.fan_duty);
    }
}

uint8_t HAL_ReadPump(void) {
    return hw.pump_duty;
}

uint8_t HAL_ReadFan(void) {
    return hw.fan_duty;
}

