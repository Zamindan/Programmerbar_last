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
#include "globals.h"
#include "config.h"

static const char *TAG = "SAFETY_TASK";

void safety_task(void *paramater)
{
    // Initialise structs
    SafetyData safety_data = {
        .max_current_user = 0,
        .max_power_user = 0,
        .max_temperature_user = 0,
        .min_voltage_user = 0};
    MeasurementData measurements;

    // Set the pins controlling relays high, meaning the relays are closed since they are NO.
    gpio_set_direction(POWER_SWITCH_RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(DUT_RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(POWER_SWITCH_RELAY_PIN, 1);
    gpio_set_level(DUT_RELAY_PIN, 0);


    while (1)
    {
        // Check safety queue
        if (xQueuePeek(safety_queue, &safety_data, pdMS_TO_TICKS(10)))
        {
            // Check measurement queue
            if (xQueuePeek(measurement_queue, &measurements, pdMS_TO_TICKS(10)))
            {
                // Check if bus voltage is higher than max user or hardcoded max voltage
                if ((measurements.bus_voltage > safety_data.max_voltage_user) || (measurements.bus_voltage > MAX_VOLTAGE))
                {
                    xEventGroupSetBits(safety_event_group, OVERVOLTAGE_BIT);
                    gpio_set_level(POWER_SWITCH_RELAY_PIN, 0);
                    gpio_set_level(DUT_RELAY_PIN, 0);
                }

                // Check if current is higher than max user or hardcoded max current
                if ((measurements.current > safety_data.max_current_user) || (measurements.current > MAX_CURRENT))
                {
                    xEventGroupSetBits(safety_event_group, OVERCURRENT_BIT);
                    gpio_set_level(POWER_SWITCH_RELAY_PIN, 0);
                    gpio_set_level(DUT_RELAY_PIN, 0);
                }

                // Check if temperature is higher than user or hardcoded max temperature
                if ((measurements.temperature > safety_data.max_temperature_user) || (measurements.temperature > MAX_TEMPERATURE))
                {
                    xEventGroupSetBits(safety_event_group, OVERTEMPERATURE_BIT);
                    gpio_set_level(POWER_SWITCH_RELAY_PIN, 0);
                    gpio_set_level(DUT_RELAY_PIN, 0);
                }

                // Check if bus voltage is lower than user set minimum voltage
                if ((measurements.bus_voltage < safety_data.min_voltage_user))
                {
                    xEventGroupSetBits(safety_event_group, UNDERVOLTAGE_BIT);
                    gpio_set_level(POWER_SWITCH_RELAY_PIN, 0);
                    gpio_set_level(DUT_RELAY_PIN, 0);
                }
            }
        }
    }
}