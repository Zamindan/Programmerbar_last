#ifndef SAFETY_TASK_H
#define SAFETY_TASK_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

//Bits for protection triggering
#define VOLTAGE_BIT 1 << 0
#define CURRENT_BIT 1 << 1
#define POWER_BIT 1 << 2
#define TEMPERATURE_BIT 1 << 3
#define 

//Data structure for safety data
typedef struct {
    float max_voltage; 
    float min_voltage;       
    float max_current;        
    float max_power;          
    float max_temperature;    
} SafetyData;

extern QueueHandle_t safety_queue;

extern EventGroupHandle_t safety_event_group;

#endif