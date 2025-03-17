#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "safety_task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "measurement_task.h"
#include "control_task.h"
#include "hmi_task.h"
#include "communication_task.h"

#define POWER_RELAY_PIN GPIO_NUM_45

static const char *TAG = "SAFETY_TASK";

void safety_task(void *paramater)
{
    // Initialise structs
    SafetyData safety_data = {
        .max_current_hard = 11,
        .max_current_user = 0,
        .max_temperature_hard = 100,
        .max_power_user = 0,
        .max_temperature_user = 0,
        .max_voltage_hard = 50,
        .max_voltage_user = 0
    };
    MeasurementData measurements;

    while (1)
    {
        
        if (xQueuePeek(safety_queue, &safety_data, pdMS_TO_TICKS(1)) == pdTRUE)
        {
        }
        if ((xEventGroupGetBits(signal_event_group) & HMI_SETPOINT_BIT) | (xEventGroupGetBits(signal_event_group) & COMMUNICATION_SETPOINT_BIT) | (xEventGroupGetBits(signal_event_group) & CONTROL_SETPOINT_BIT))
        {
            if (xQueuePeek(measurement_queue, &measurements, pdMS_TO_TICKS(1)) == pdTRUE)
            {
                if ((measurements.voltage > safety_data.max_voltage_user) || (measurements.voltage > safety_data.max_voltage_hard))
                {
                    ESP_LOGE(TAG, "Voltage exceeded.");
                    xEventGroupSetBits(signal_event_group, OVERVOLTAGE_BIT);
                }
                if (measurements.voltage < safety_data.min_voltage_user)
                {
                    ESP_LOGE(TAG, "Voltage below minimum.");
                    xEventGroupSetBits(signal_event_group, OVERVOLTAGE_BIT);
                }
                if ((measurements.current > safety_data.max_current_user) || (measurements.current > safety_data.max_current_user))
                {
                    ESP_LOGE(TAG, "Current exceeded.");
                    xEventGroupSetBits(signal_event_group, OVERCURRENT_BIT);
                }
                if (measurements.power > safety_data.max_power_user)
                {
                    ESP_LOGE(TAG, "Power exceeded.");
                    xEventGroupSetBits(signal_event_group, OVERPOWER_BIT);
                }
                if ((measurements.temperature > safety_data.max_temperature_user) || (measurements.temperature > safety_data.max_temperature_user))
                {
                    ESP_LOGE(TAG, "Temperature exceeded.");
                    xEventGroupSetBits(signal_event_group, OVERTEMPERATURE_BIT);
                }
            }
        }
    }
}