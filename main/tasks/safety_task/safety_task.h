#ifndef SAFETY_TASK_H
#define SAFETY_TASK_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

//Bits for protection triggering
#define OVERVOLTAGE_BIT 1 << 0
#define OVERCURRENT_BIT 1 << 1
#define UNDERVOLTAGE_BIT 1 << 2
#define OVERTEMPERATURE_BIT 1 << 3

// GPIO for relays
#define BATTERY_RELAY_PIN 48
#define POWER_SWITCH_RELAY_PIN 45

//Data structure for safety data
typedef struct {
    float max_voltage_user;
    float min_voltage_user;
    float max_current_user;
    float max_power_user;
    float max_temperature_user;
} SafetyData;

extern QueueHandle_t safety_queue;

extern EventGroupHandle_t hmi_safety_event_group;

#endif