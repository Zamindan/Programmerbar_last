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
#include "communication_task.h"
#include "pwm.h"
#include "driver/ledc.h"
#include "safety_task.h"
#include "globals.h"
#include "config.h"
#include "safety_task.h"

// Declare queues
QueueHandle_t mode_queue;        // Declare queue that holds mode.
QueueHandle_t setpoint_queue;    // Declare queue that holds setpoint.
QueueHandle_t measurement_queue; // Declare queue that holds measurement struct.
QueueHandle_t safety_queue;      // Declare queue that holds max values for different parameters, set by user and hardcoded.

// Declare event groups
EventGroupHandle_t signal_event_group; // Declare event group that has bits related to signaling if setpoint data has been updated
EventGroupHandle_t safety_event_group; // Declare event group that has bits related to triggering of over-current, voltage, temperature and power. Also has start/stop bit.

static const char *TAG = "MAIN";

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
    xTaskCreatePinnedToCore(communication_task, "Communication Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(control_task, "Control Task", 4096, NULL, 2, NULL, 1);

    // Start WiFi and HTTP server
    wifi_start();
    start_webserver();
}