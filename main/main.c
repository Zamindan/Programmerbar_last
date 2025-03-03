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


static const char *TAG = "MAIN";

float current = 0;
float voltage = 0;
float power = 0;
float temperature = 0;

float set_current = 0;
float set_voltage = 0;
float set_power = 0;

LOAD_MODE_t load_mode = MODE_CC;

void app_main(void)
{
    void rtc_task(void *parameter);
    void measurements_task(void *parameter);
    void control_task(void *parameter);

    wifi_start();

    xTaskCreatePinnedToCore(rtc_task, "rtc_task", 4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(measurements_task, "measurements_task", 4096, NULL, 2, NULL, 0);

    // Initialize control
    control_init();
    xTaskCreatePinnedToCore(control_task, "control_task", 4096, NULL, 1, NULL, 0);

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