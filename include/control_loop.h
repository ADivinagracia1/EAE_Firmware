#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include <stdbool.h>

void Control_Init(float loop_period_s);
void Control_Loop(void);

#endif // CONTROL_LOOP_H