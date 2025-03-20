#ifndef MEASUREMENT_TASK_H
#define MEASUREMENT_TASK_H
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t measurement_queue;

// Processed measurements
typedef struct {
    float bus_voltage;    // V
    float current;        // A
    float power;          // W
    float temperature;    // °C
} MeasurementData;

void measurement_task(void *pvParameters);


#endif