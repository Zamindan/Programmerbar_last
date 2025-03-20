#ifndef GLOBALS_H
#define GLOBALS_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

// Queues
extern QueueHandle_t mode_queue;
extern QueueHandle_t setpoint_queue;
extern QueueHandle_t safety_queue;
extern QueueHandle_t measurement_queue;

// Event Groups
extern EventGroupHandle_t signal_event_group;
extern EventGroupHandle_t safety_event_group;


//Data structure for safety data
typedef struct {
    float max_voltage_user;
    float min_voltage_user;
    float max_current_user;
    float max_power_user;
    float max_temperature_user;
} SafetyData;

// Data structure for processed measurements
typedef struct {
    float bus_voltage;    // V
    float current;        // A
    float power;          // W
    float temperature;    // °C
} MeasurementData;

// Data structure for modes
typedef enum {
    MODE_CC,  // Constant Current
    MODE_CV,  // Constant Voltage
    MODE_CP   // Constant Power
} ControlMode;

#endif