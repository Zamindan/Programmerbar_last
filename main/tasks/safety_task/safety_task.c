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
    SafetyData safety_data;
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
                if (measurements.voltage > safety_data.max_voltage)
                {
                    ESP_LOGE(TAG, "Voltage exceeded.");
                    xEventGroupSetBits(signal_event_group, VOLTAGE_BIT);
                }
                if (measurements.voltage < safety_data.min_voltage)
                {
                    ESP_LOGE(TAG, "Voltage below minimum.");
                    xEventGroupSetBits(signal_event_group, VOLTAGE_BIT);
                }
                if (measurements.current > safety_data.max_current)
                {
                    ESP_LOGE(TAG, "Current exceeded.");
                    xEventGroupSetBits(signal_event_group, CURRENT_BIT);
                }
                if (measurements.power > safety_data.max_power)
                {
                    ESP_LOGE(TAG, "Power exceeded.");
                    xEventGroupSetBits(signal_event_group, POWER_BIT);
                }
                if (measurements.temperature > safety_data.max_temperature)
                {
                    ESP_LOGE(TAG, "Temperature exceeded.");
                    xEventGroupSetBits(signal_event_group, TEMPERATURE_BIT);
                }
            }
        }
    }
}