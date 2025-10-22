#include "hal.h"
#include "state_machine.h"
#include "control_loop.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int main(int argc, char *argv[]) {
    float initial_temp = 20.0f;  // default temperature
    float loop_period_s = 1.0f;  // default loop period in seconds

    if (argc > 1) {
        initial_temp = atof(argv[1]);  // convert string to float
        if (initial_temp < 20.0f) initial_temp = 20.0f;  // clamp min
        if (initial_temp > 100.0f) initial_temp = 100.0f; // clamp max
    }

    if (argc > 2) {
        loop_period_s = atof(argv[2]);
        if (loop_period_s < 0.05f) loop_period_s = 0.05f;  // min 10ms
        if (loop_period_s > 5.0f) loop_period_s = 5.0f;  // max 10s
    }

    HAL_Init();
    SM_Init();
    Control_Init(loop_period_s);

    HAL_SetInitialTemperature(initial_temp);

    printf("Starting control loop (%.2fs period, initial temp %.2f°C)\n",
           loop_period_s, initial_temp);

    while (true) {
        Control_Loop();
        usleep(loop_period_s * 1e6);  
    }

    return 0;
}

