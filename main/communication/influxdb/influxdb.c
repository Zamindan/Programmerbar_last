#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_timer.h"


// InfluxDB Cloud configuration
#define INFLUXDB_URL "https://us-west-2-1.aws.cloud2.influxdata.com/api/v2/write"
#define INFLUXDB_TOKEN "r2wMy-BjI6jY3OIoFfObNckIFE0S1Tb8yMAZXEqHeKqvV8wATIXPw36udLSpelVhDsBr0pEBDwBTo05t9Kemug=="
#define INFLUXDB_ORG "ce586bc50b41e385"
#define INFLUXDB_BUCKET "esp32_data"


// Tag for logging
static const char *TAG = "INFLUXDB";


// Send data to InfluxDB
static void send_to_influxdb(float current, float voltage, float power, float temperature) {
    char payload[256];
    snprintf(payload, sizeof(payload),
             "measurements,sensor_id=esp32-s3 current=%.2f,voltage=%.2f,power=%.2f,temperature=%.2f",
             current, voltage, power, temperature);

    esp_http_client_config_t config = {
        .url = INFLUXDB_URL,
        .method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, "Authorization", INFLUXDB_TOKEN);
    esp_http_client_set_header(client, "Content-Type", "text/plain");
    esp_http_client_set_post_field(client, payload, strlen(payload));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Data sent to InfluxDB: %s", payload);
    } else {
        ESP_LOGE(TAG, "Failed to send data to InfluxDB: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

// Main application
void app_main(void) {

    // Simulate sensor data (replace with actual sensor readings)
    float current = 2.5;      // Example current value
    float voltage = 5.0;      // Example voltage value
    float power = 12.5;       // Example power value
    float temperature = 25.0; // Example temperature value

    // Send data to InfluxDB every 5 seconds
    while (1) {
        send_to_influxdb(current, voltage, power, temperature);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}





