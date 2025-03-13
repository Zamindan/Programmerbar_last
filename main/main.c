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

QueueHandle_t mode_queue;
QueueHandle_t setpoint_queue;
QueueHandle_t measurement_queue;

EventGroupHandle_t signal_event_group;

static const char *TAG = "MAIN";


void app_main()
{
    pwm_init();
    for (int i = 0; i <= 1024; i++)
    {
        float duty_cycle = (float)i / 1024.0;
        pwm_update_duty(duty_cycle);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
/*
    signal_event_group = xEventGroupCreate();
    
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

    xTaskCreatePinnedToCore(measurement_task, "Measurement Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(hmi_task, "HMI Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(communication_task, "Communication Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(control_task, "Control Task", 4096, NULL, 1, NULL, 1);

    wifi_start();
    start_webserver();
    */
}