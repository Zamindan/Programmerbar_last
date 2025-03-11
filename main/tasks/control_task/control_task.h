#ifndef CONTROL_TASK_H
#define CONTROL_TASK_H

typedef enum {
    MODE_CC,  // Constant Current
    MODE_CV,  // Constant Voltage
    MODE_CP   // Constant Power
} ControlMode;

extern float setpoint;

void control_task(void *pvParameters);

#endif