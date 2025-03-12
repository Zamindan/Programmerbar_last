#ifndef HMI_TASK_H
#define HMI_TASK_H

extern float setpoint;

typedef enum {
    MODE_CC,  // Constant Current
    MODE_CV,  // Constant Voltage
    MODE_CP   // Constant Power
} ControlMode;

extern QueueHandle_t mode_queue;
extern QueueHandle_t setpoint_queue;


void hmi_task(void *pvParameters);

#endif