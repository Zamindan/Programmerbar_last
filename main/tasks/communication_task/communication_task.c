#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "hmi_task.h"
#include "communication_task.h"
#include "measurement_task.h"

#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_http_server.h"
#include "esp_log.h"

#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define TCP_SUCCESS 1 << 0
#define TCP_FAILURE 1 << 1
#define MAX_FAILURES 10

#define CONFIG_WIFI_SSID "ESP"
#define CONFIG_WIFI_PASSWORD "merethe123"

static const char *TAG = "COMMUNICATION_TASK";
static const char *index_html = 
"<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"    <meta charset=\"UTF-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"    <title>ESP32 Measurements</title>"
"    <style>"
"        body {"
"            font-family: Arial, sans-serif;"
"            margin: 0;"
"            padding: 0;"
"            display: flex;"
"            flex-direction: column;"
"            align-items: center;"
"            justify-content: center;"
"            height: 100vh;"
"            background-color: #f0f0f0;"
"        }"
"        .container {"
"            background: white;"
"            padding: 20px;"
"            border-radius: 8px;"
"            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);"
"        }"
"        h1 {"
"            margin-bottom: 20px;"
"        }"
"        .measurement {"
"            margin-bottom: 10px;"
"        }"
"        .setpoint {"
"            margin-top: 20px;"
"        }"
"    </style>"
"</head>"
"<body>"
"    <div class=\"container\">"
"        <h1>ESP32 Measurements</h1>"
"        <div class=\"measurement\">"
"            <strong>Voltage:</strong> <span id=\"voltage\">0.00</span> V"
"        </div>"
"        <div class=\"measurement\">"
"            <strong>Current:</strong> <span id=\"current\">0.00</span> A"
"        </div>"
"        <div class=\"measurement\">"
"            <strong>Power:</strong> <span id=\"power\">0.00</span> W"
"        </div>"
"        <div class=\"measurement\">"
"            <strong>Temperature:</strong> <span id=\"temperature\">0.00</span> °C"
"        </div>"
"        <div class=\"setpoint\">"
"            <label for=\"setpoint\">Setpoint:</label>"
"            <input type=\"number\" id=\"setpoint\" step=\"0.01\">"
"            <button onclick=\"updateSetpoint()\">Update</button>"
"        </div>"
"    </div>"
"    <script>"
"        async function fetchMeasurements() {"
"            const response = await fetch('/measurement');"
"            const data = await response.json();"
"            document.getElementById('voltage').textContent = data.voltage.toFixed(2);"
"            document.getElementById('current').textContent = data.current.toFixed(2);"
"            document.getElementById('power').textContent = data.power.toFixed(2);"
"            document.getElementById('temperature').textContent = data.temperature.toFixed(2);"
"        }"
"        async function updateSetpoint() {"
"            const setpoint = document.getElementById('setpoint').value;"
"            await fetch('/setpoint', {"
"                method: 'POST',"
"                headers: {"
"                    'Content-Type': 'application/x-www-form-urlencoded',"
"                },"
"                body: setpoint,"
"            });"
"        }"
"        setInterval(fetchMeasurements, 1000);"
"    </script>"
"</body>"
"</html>";





// Event group to contain status information
static EventGroupHandle_t wifi_event_group;

// Retry tracker
static int s_retry_num = 0;


// Wifi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Connecting to AP...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < MAX_FAILURES)
        {
            ESP_LOGI(TAG, "Retrying connection to AP...");
            esp_wifi_connect();
            s_retry_num++;
        }
        else
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
        }
    }
}

static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
    }
}

esp_err_t wifi_init()
{
    int status = WIFI_FAILURE;

    // Initialise the esp network interface
    ESP_ERROR_CHECK(esp_netif_init());

    // Initialise default esp event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create wifi station in the wifi driver
    esp_netif_create_default_wifi_sta();

    // setup wifi station with the default wifi config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &wifi_handler_event_instance));

    esp_event_handler_instance_t ip_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &ip_event_handler,
                                                        NULL,
                                                        &ip_handler_event_instance));

    /// Start the WIFI driver

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    // Set the WiFi controller to be in station mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Set WiFi configuration
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Start the WiFi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA init complete");

    // Wait for the connection to be complete or fail
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_SUCCESS | WIFI_FAILURE, pdFALSE, pdFALSE, portMAX_DELAY);

    // xEvenGroupWaitBits() returns the bits before the call returned, so we test which event happened

    if (bits & WIFI_SUCCESS)
    {
        ESP_LOGI(TAG, "Connected to AP");
        status = WIFI_SUCCESS;
    }
    else if (bits & WIFI_FAILURE)
    {
        ESP_LOGI(TAG, "Failed to connect to AP");
        status = WIFI_FAILURE;
    }
    else
    {
        ESP_LOGE(TAG, "Unexpected event");
        status = WIFI_FAILURE;
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, ip_handler_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

    return status;
}

void wifi_start()
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
    status = wifi_init();
    if (WIFI_SUCCESS != status)
    {
        ESP_LOGI(TAG, "Failed to associate to AP, dying...");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "Connected to AP");
    }
}

static esp_err_t index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, strlen(index_html));
    return ESP_OK;
}

static esp_err_t get_measurement_handler(httpd_req_t *req) {
    MeasurementData measurement;
    if (xQueuePeek(measurement_queue, &measurement, pdMS_TO_TICKS(10)) == pdTRUE) {
        char resp[100];
        snprintf(resp, sizeof(resp), "{\"voltage\": %.2f, \"current\": %.2f, \"power\": %.2f, \"temperature\": %.2f}",
                 measurement.bus_voltage, measurement.current, measurement.power, measurement.temperature);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, resp, strlen(resp));
    } else {
        httpd_resp_send_500(req);
    }
    return ESP_OK;
}

static esp_err_t set_setpoint_handler(httpd_req_t *req) {
    char content[100];
    size_t recv_size = MIN(req->content_len, sizeof(content) - 1);
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    content[recv_size] = '\0';

    float setpoint = atof(content);
    xQueueOverwrite(setpoint_queue, &setpoint);

    httpd_resp_send(req, "Setpoint updated", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_handle_t start_webserver()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    esp_err_t ret = httpd_start(&server, &config);
    if (ret == ESP_OK) {
        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &index_uri);

        httpd_uri_t measurement_uri = {
            .uri       = "/measurement",
            .method    = HTTP_GET,
            .handler   = get_measurement_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &measurement_uri);

        httpd_uri_t setpoint_uri = {
            .uri       = "/setpoint",
            .method    = HTTP_POST,
            .handler   = set_setpoint_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &setpoint_uri);
    } else {
        ESP_LOGI(TAG, "Server failed to start");
    }
    return server;
}


void communication_task(void *parameter)
{   
    float setpoint = 0.0;
    float previous_setpoint = 0.0;
    ControlMode mode = MODE_CC;
    MeasurementData measurement;
    wifi_start();
    start_webserver();



    while (1)
    {
        if (setpoint != previous_setpoint)
        {
            previous_setpoint = setpoint;
            xQueueOverwrite(setpoint_queue, &setpoint);
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else if (xQueuePeek(setpoint_queue, &setpoint, pdTICKS_TO_MS(1)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received setpoint data");
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}