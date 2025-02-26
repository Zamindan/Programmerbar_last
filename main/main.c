#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "measurements.h"
#include "esp_log.h"
#include "wifi.h"
#include "nvs_flash.h"
#include "time.h"
#include "esp_sntp.h"
#include "esp_http_server.h"
#include "rtc_time.h"


static const char *TAG = "MAIN";

// Shared variable to store the current time
char current_time_str[64];
SemaphoreHandle_t time_mutex;

// HTTP GET handler
esp_err_t hello_get_handler(httpd_req_t *req)
{
    char time_str[64];

    // Get the current time from the shared variable
    if (xSemaphoreTake(time_mutex, portMAX_DELAY))
    {
        strncpy(time_str, current_time_str, sizeof(time_str));
        xSemaphoreGive(time_mutex);
    }

    // Send the current time as the response
    httpd_resp_send(req, time_str, strlen(time_str));
    return ESP_OK;
}

// Function to start the web server
httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        httpd_uri_t hello_uri = {
            .uri       = "/hello",
            .method    = HTTP_GET,
            .handler   = hello_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &hello_uri);
    }
    return server;
}

void app_main(void)
{
    //void measurements_task(void *parameter);
    //   Create a task for the measurements
    //xTaskCreatePinnedToCore(measurements_task, "measurements_task", 4096, NULL, 5, NULL, 1);
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

    // Initialize the semaphore
    time_mutex = xSemaphoreCreateMutex();
    if (time_mutex == NULL)
    {
        ESP_LOGI(TAG, "Failed to create mutex");
        return;
    }

    void rtc_task(void *parameter);

    xTaskCreatePinnedToCore(rtc_task, "rtc_task", 4096, NULL, 5, NULL, 0);

    // Start the web server
    start_webserver();
    ESP_LOGI(TAG, "Web server started");
}
