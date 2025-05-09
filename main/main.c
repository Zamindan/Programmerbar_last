#include <stdio.h>
#include "measurement_task.h"
#include "control_task.h"
#include "hmi_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "measurement_task.h"
#include "control_task.h"
#include "esp_log.h"
#include "adc.h"
#include "driver/gpio.h"
#include "i2c.h"
#include "pwm.h"
#include "driver/ledc.h"
#include "safety_task.h"
#include "globals.h"
#include "config.h"
#include "safety_task.h"
#include "wifi.h"
#include "http_server.h"

/**
 * @file main.c
 * @brief Entry point for the programmable electrical load project.
 *
 * This file contains the `app_main` function, which initializes the system,
 * creates FreeRTOS queues and event groups, and starts the various tasks
 * (safety, measurement, HMI, communication, and control). It also starts
 * the WiFi module and HTTP server for remote interaction.
 *
 * @author Sondre
 * @date 2025-03-24
 */

// Declare queues
QueueHandle_t mode_queue;        /**< Queue for control mode updates. */
QueueHandle_t setpoint_queue;    /**< Queue for setpoint values. */
QueueHandle_t measurement_queue; /**< Queue for processed measurement data. */
QueueHandle_t safety_queue;      /**< Queue for safety-related data. */

// Declare event groups
EventGroupHandle_t signal_event_group; /**< Event group for signaling between tasks. */
EventGroupHandle_t safety_event_group; /**< Event group for safety-related events. */

static const char *TAG = "MAIN";

/**
 * @brief Main entry point for the application.
 *
 * This function initializes the system by creating FreeRTOS queues and event groups,
 * and starting the various tasks. It also initializes the WiFi module and HTTP server
 * for remote interaction.
 */
void app_main()
{
    // Create event groups
    signal_event_group = xEventGroupCreate();
    safety_event_group = xEventGroupCreate();

    // Create queues
    safety_queue = xQueueCreate(1, sizeof(SafetyData));
    if (safety_queue == NULL)
    {
        ESP_LOGE(TAG, "Safety queue failed to create.");
    }
    else
    {
        ESP_LOGI(TAG, "Safety queue created.");
    }

    setpoint_queue = xQueueCreate(1, sizeof(float));
    if (setpoint_queue == NULL)
    {
        ESP_LOGE(TAG, "Setpoint queue failed to create.");
    }
    else
    {
        ESP_LOGI(TAG, "Setpoint queue created.");
    }

    mode_queue = xQueueCreate(1, sizeof(ControlMode));
    if (mode_queue == NULL)
    {
        ESP_LOGE(TAG, "Mode queue failed to create.");
    }
    else
    {
        ESP_LOGI(TAG, "Mode queue created.");
    }

    measurement_queue = xQueueCreate(1, sizeof(MeasurementData));
    if (measurement_queue == NULL)
    {
        ESP_LOGE(TAG, "Measurement queue failed to create.");
    }
    else
    {
        ESP_LOGI(TAG, "Measurement queue created.");
    }

    // Set tasks to cores
    xTaskCreatePinnedToCore(safety_task, "Safety Task", 4096, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(measurement_task, "Measurement Task", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(hmi_task, "HMI Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(control_task, "Control Task", 4096, NULL, 2, NULL, 1);

    // Start WiFi and HTTP server
    wifi_start();      // This module starts its own FreeRTOS task through esp_wifi_start() with priority 23 and an event loop task with priority 20.
    start_webserver(); // This module starts its own FreeRTOS task through httpd_start() with priority 5.

}