#include "time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_sntp.h"
#include "esp_log.h"

static const char *TAG = "RTC_TIME";

// Shared variable to store the current time
extern char current_time_str[64];
extern SemaphoreHandle_t time_mutex;

void rtc_task(void *parameter)
{

    // Initialise SNTP
    ESP_LOGI(TAG, "Initialising SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();

    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo;
    int retry = 0;
    const int retry_count = 10;

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(pdTICKS_TO_MS(1000));
    }

    // Set timezone to Oslo time (CET/CEST)
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    tzset();

    // Display the current time in a loop
    while (true)
    {
        time(&now);
        localtime_r(&now, &timeinfo);
        char strftime_buf[64];
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        // Update the shared variable with the current time
        if (xSemaphoreTake(time_mutex, portMAX_DELAY))
        {
            strncpy(current_time_str, strftime_buf, sizeof(current_time_str));
            xSemaphoreGive(time_mutex);
        }
        ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
        vTaskDelay(pdTICKS_TO_MS(900));
    }
}