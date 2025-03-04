#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "pwm.h"
#include "measurements.h"
#include "wifi.h"
#include "nvs_flash.h"
#include "time.h"
#include "esp_sntp.h"
#include "rtc_time.h"
#include "web_server.h"
#include "control.h"
#include "esp_log.h"
#include "control.h"
//TEST

static const char *TAG = "MAIN";

load_mode_t load_mode = MODE_CC;

SemaphoreHandle_t time_mutex;

char current_time_str[64];


struct measurement_data measurements;
struct control_data control_signals;

struct measurement_data simulated_data = {0};
SemaphoreHandle_t sensor_mutex;

void sensor_sim_task(void *pvParameter) {

    struct measurement_data *measurements = (struct measurement_data *)pvParameter;
    // Seed random number generator
    srand(time(NULL));

    while(1) {
        // Generate random values within realistic ranges
        float new_current = 0.5f + (rand() % 950) / 100.0f;  // 0.5-10A
        float new_voltage = 10.0f + (rand() % 380) / 10.0f;   // 10-48V
        float new_power = new_current * new_voltage;          // Calculate power
        float new_temp = 25.0f + (rand() % 250) / 10.0f;      // 25-50°C

        // Protect shared data with mutex
        if(xSemaphoreTake(sensor_mutex, pdMS_TO_TICKS(100))) {
            measurements->current = new_current;
            measurements->voltage = new_voltage;
            measurements->power = new_power;
            measurements->temperature = new_temp;
            xSemaphoreGive(sensor_mutex);
            
            ESP_LOGI(TAG, "New values: %.2fA, %.2fV, %.2fW, %.1f°C", 
                    new_current, new_voltage, new_power, new_temp);
        } else {
            ESP_LOGW(TAG, "Failed to update sensor data - mutex timeout");
        }

        // Update every 2 seconds
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Call this from app_main() to initialize
void init_sensor_sim() {
    sensor_mutex = xSemaphoreCreateMutex();
    if(sensor_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create sensor mutex!");
        return;
    }

    xTaskCreatePinnedToCore(
        sensor_sim_task,    // Task function
        "sensor_sim",       // Task name
        4096,               // Stack size
        &measurements,               // Parameters
        2,                  // Priority
        NULL,               // Task handle
        tskNO_AFFINITY      // Core
    );
}


void app_main(void)
{
    time_mutex = xSemaphoreCreateMutex();
    if (time_mutex == NULL)
    {
        ESP_LOGE(TAG, "FATAL: Failed to create time mutex");
        vTaskDelay(pdMS_TO_TICKS(100));
        esp_restart(); // Critical failure, restart
    }

    wifi_start();
    xTaskCreatePinnedToCore(rtc_task, "rtc_task", 4096, NULL, 3, NULL, 0); // Max priroty is 25!
    start_webserver(&measurements);
    init_sensor_sim();
}



/*
void app_main(void)
{
    esp_err_t status = WIFI_FAILURE;
    // initialize storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // connect to wireless AP
    status = connect_wifi();
    if (WIFI_SUCCESS != status)
    {
        ESP_LOGI(TAG, "Failed to associate to AP, dying...");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "Connected to AP");
    }

    mode_mutex = xSemaphoreCreateMutex();
    if (mode_mutex == NULL)
    {
        ESP_LOGI(TAG, "Failed to create mode mutex");
        return;
    }

    void rtc_task(void *parameter);
    void measurements_task(void *parameter);

    xTaskCreatePinnedToCore(rtc_task, "rtc_task", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(measurements_task, "measurements_task", 4096, NULL, 5, NULL, 0);

    // Initialize control
    control_init();
    xTaskCreatePinnedToCore(control_task, "control_task", 4096, NULL, 5, NULL, 0);

    // Start the web server
    start_webserver();
    ESP_LOGI(TAG, "Web server started");
}*/