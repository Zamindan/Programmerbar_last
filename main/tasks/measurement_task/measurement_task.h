#ifndef MEASUREMENT_TASK_H
#define MEASUREMENT_TASK_H
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Processed measurements
typedef struct {
    float bus_voltage;    // V
    float current;        // A
    float power;          // W
    float temperature;    // °C
} MeasurementData;

void measurement_task(void *pvParameters);
extern QueueHandle_t measurement_queue;

#endif