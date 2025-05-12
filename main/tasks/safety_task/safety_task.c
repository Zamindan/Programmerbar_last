#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "safety_task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "measurement_task.h"
#include "control_task.h"
#include "hmi_task.h"
#include "globals.h"
#include "config.h"

/**
 * @file safety_task.c
 * @brief Implementation of the safety task.
 *
 * This file contains the implementation of the safety task, which is responsible
 * for monitoring safety conditions such as overvoltage, overcurrent, overtemperature,
 * and undervoltage. The task interacts with other tasks via FreeRTOS queues and
 * event groups to ensure safe operation of the system. If a safety condition is
 * violated, the task disables the relays to protect the system.
 *
 * @note The relays are configured as normally open (NO), meaning they are closed
 *       when the GPIO pin is set high.
 *
 * @date 2025-05-12
 */

static const char *TAG = "SAFETY_TASK"; /**< Tag for logging messages from the safety task. */

/**
 * @brief Safety task for monitoring and enforcing safety conditions.
 *
 * This task continuously monitors measurement data and user-defined safety limits.
 * If a safety condition is violated (e.g., overvoltage, overcurrent), it triggers
 * the appropriate safety event and disables the relays to protect the system.
 *
 * @param paramater Pointer to task parameters (can be NULL).
 */
void safety_task(void *paramater)
{
    // Initialise structs
    SafetyData safety_data = {
        .max_current_user = 0,
        .max_power_user = 0,
        .max_temperature_user = 0,
        .min_voltage_user = 0,
        .soft_max_current = 0,
        .soft_max_voltage = 0,
        .soft_max_temperature = 0
    };
    
    MeasurementData measurements; /**< Struct to hold the latest measurement data. */

    // Set the pins controlling relays high, meaning the relays are closed since they are NO.
    gpio_set_direction(POWER_SWITCH_RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(DUT_RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(POWER_SWITCH_RELAY_PIN, 1);
    gpio_set_level(DUT_RELAY_PIN, 0);


    while (1)
    {
        // Check safety queue for updated safety limits
        if (xQueuePeek(safety_queue, &safety_data, pdMS_TO_TICKS(10)))
        {
            // Check measurement queue for the latest measurement data
            if (xQueuePeek(measurement_queue, &measurements, pdMS_TO_TICKS(10)))
            {
                // Check if bus voltage exceeds user-defined or hardcoded maximum voltage
                if ((measurements.bus_voltage > safety_data.max_voltage_user) || (measurements.bus_voltage > MAX_VOLTAGE))
                {
                    xEventGroupSetBits(safety_event_group, OVERVOLTAGE_BIT);
                    gpio_set_level(POWER_SWITCH_RELAY_PIN, 0);
                    gpio_set_level(DUT_RELAY_PIN, 0);
                }
                // Check if current exceeds user-defined or hardcoded maximum current
                else if ((measurements.current > safety_data.max_current_user) || (measurements.current > MAX_CURRENT))
                {
                    xEventGroupSetBits(safety_event_group, OVERCURRENT_BIT);
                    gpio_set_level(POWER_SWITCH_RELAY_PIN, 0);
                    gpio_set_level(DUT_RELAY_PIN, 0);
                }
                // Check if temperature exceeds user-defined or hardcoded maximum temperature
                else if ((measurements.temperature_internal > safety_data.max_temperature_user) || (measurements.temperature_internal > MAX_TEMPERATURE))
                {
                    xEventGroupSetBits(safety_event_group, OVERTEMPERATURE_BIT);
                    ESP_LOGE(TAG, "Overtemperature detected: %.2f °C (Limit: %.2f °C)", measurements.temperature_internal, safety_data.max_temperature_user);
                    gpio_set_level(POWER_SWITCH_RELAY_PIN, 0);
                    gpio_set_level(DUT_RELAY_PIN, 0);
                }
                // Check if bus voltage is below user-defined minimum voltage
                else if ((measurements.bus_voltage < safety_data.min_voltage_user))
                {
                    xEventGroupSetBits(safety_event_group, UNDERVOLTAGE_BIT);
                    gpio_set_level(POWER_SWITCH_RELAY_PIN, 0);
                    gpio_set_level(DUT_RELAY_PIN, 0);
                }

                if (xEventGroupGetBits(safety_event_group) != 0)
                {
                    xEventGroupClearBits(signal_event_group, START_STOP_BIT);
                }
            }
        }

        if (xEventGroupGetBits(signal_event_group) & RESET_BIT)
        {
            gpio_set_level(POWER_SWITCH_RELAY_PIN, 1);
            xEventGroupClearBits(safety_event_group, OVERVOLTAGE_BIT);
            xEventGroupClearBits(safety_event_group, OVERCURRENT_BIT);
            xEventGroupClearBits(safety_event_group, OVERTEMPERATURE_BIT);
            xEventGroupClearBits(safety_event_group, UNDERVOLTAGE_BIT);
            xEventGroupClearBits(signal_event_group, START_STOP_BIT);
        }

        // Small delay to prevent task starvation
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}