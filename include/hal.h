#ifndef HAL_H
#define HAL_H

#include <stdbool.h>
#include <stdint.h>

void HAL_Init(void);

// Inputs
bool HAL_ReadIgnition(void);
float HAL_ReadTemperature(void);
void HAL_SetInitialTemperature(float temp);

// Outputs
void HAL_SetPump(uint8_t duty_cycle);
void HAL_SetFan(uint8_t duty_cycle);
uint8_t HAL_ReadPump(void);
uint8_t HAL_ReadFan(void);

// CAN In/Out
void HAL_SendCANMessage(uint32_t id, float value);
bool HAL_ReadCANMessage(uint32_t *id, float *value);
void HAL_GenerateCANMessage(uint32_t id, float value);

#endif // HAL_H