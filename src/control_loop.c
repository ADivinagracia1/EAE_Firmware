#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "control_loop.h"
#include "hal.h"
#include "state_machine.h" 

/*
    Usage of PID in the State Machine:
        OFF -> 0%
    STANDBY -> 10%
    COOLING -> PID-computed value
    OVERHEAT -> 100%
*/

// ---------------- PID Structure ----------------
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float prev_error;
    float integral;
} PID_Controller_t;

 
// In reality, the pump and fan would have their own variables based on
// the mechanical characteristics. For simplification in this assignment, 
// the pump and the fan share the same duty cycle
static PID_Controller_t pid_fanpump;      // Single PID for both fan and pump.
static float cooling_setpoint = 70.0f;  
static float dt = 1.0f;
static float pwm = 0.0f; 

static void PID_Init(PID_Controller_t *pid, float Kp, float Ki, float Kd){
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
}

static float PID_Compute(PID_Controller_t *pid, float setpoint, float measurement) {
    const float error = setpoint - measurement;
    pid->integral += error * dt;
    float derivative = (error - pid->prev_error) / dt;
    pid->prev_error = error;


    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;

    printf("Checking PID value: output_pwm = %.02f\n", output);

    // Clamp to 0-100%
    output = fmin(fmax(output, 0.0f), 100.0f);

    return output;
}

void Control_Init(float loop_period_s) {
    dt = loop_period_s;

    // Initialize PID constants
    PID_Init(&pid_fanpump, 2.1f, 0.01f, 0.005f);

    cooling_setpoint = 70.0f;
}

void Control_Loop(void) {
    // 1. Read Sensors
    bool ignition_state = HAL_ReadIgnition();
    float coolant_temp = HAL_ReadTemperature();
    uint8_t pump_dt = HAL_ReadPump();
    uint8_t fan_dt = HAL_ReadFan();

    // 2. Update State Machine
    SM_Update(ignition_state, coolant_temp, true);

    // 3. Apply PID
    switch (SM_GetState()){
        case SystemState_STATE_OFF:
            pwm = 0.0f;
            break;
        case SystemState_STATE_STANDBY:
            pwm = 15.0f;
            break;
        case SystemState_STATE_COOLING:
            pwm = PID_Compute(&pid_fanpump, cooling_setpoint, coolant_temp);
            break;
        case SystemState_STATE_OVERHEAT:
            pwm = 100.0f;
            break;
        default:
            pwm = 0.0f;
            break;
    }

    // 4. CAN Message Handling 
    // HAL_SendCANMessage(0x100, coolant_temp); // example ID: temperature
    // HAL_SendCANMessage(0x101, pump_dt);      // example ID: pump duty
    // HAL_SendCANMessage(0x102, fan_dt);       // example ID: fan duty
    uint32_t id;
    float value;
    while (HAL_ReadCANMessage(&id, &value)) { // Poling CAN message
        printf("[CAN RX] ID: 0x%X | Value: %.2f\n", id, value);
        // NOte: Can add logic here to respond to messages
    }

    // Optional: simulate external messages (for testing)
    HAL_GenerateCANMessage(0x200, coolant_temp);

    // 5. Output to terminal 
    printf("Ignition: %s | Temperature: %.2f°C | Fan Duty Cycle: %d%% | Pump Duty Cycle: %d%%  \n",
        ignition_state ? "ON" : "OFF", 
        coolant_temp, 
        pump_dt, 
        fan_dt);
}
