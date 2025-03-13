#ifndef HMI_TASK_H
#define HMI_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define HMI_SETPOINT_BIT 1 << 0
#define COMMUNICATION_SETPOINT_BIT 1 << 1
#define CONTROL_SETPOINT_BIT 1 << 2

extern float setpoint;

typedef enum {
    MODE_CC,  // Constant Current
    MODE_CV,  // Constant Voltage
    MODE_CP   // Constant Power
} ControlMode;

extern QueueHandle_t mode_queue;
extern QueueHandle_t setpoint_queue;

extern EventGroupHandle_t signal_event_group;


void hmi_task(void *pvParameters);

#endif