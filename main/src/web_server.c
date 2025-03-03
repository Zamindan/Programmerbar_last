#include "web_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"

static const char *TAG = "WEB_SERVER";

extern float current, voltage, power, temperature;
extern float set_current, set_voltage, set_power;




















/*
// Shared variables for mode and parameters
mode_t current_mode;
float set_current = 0;
float set_voltage = 0;
float set_power = 0;
SemaphoreHandle_t mode_mutex;

// HTTP GET handler for current time
extern char current_time_str[64];
extern SemaphoreHandle_t time_mutex;

// HTTP POST handler for setting mode and parameters
esp_err_t set_mode_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    cJSON *mode = cJSON_GetObjectItem(json, "mode");
    cJSON *current = cJSON_GetObjectItem(json, "current");
    cJSON *voltage = cJSON_GetObjectItem(json, "voltage");
    cJSON *power = cJSON_GetObjectItem(json, "power");

    if (xSemaphoreTake(mode_mutex, portMAX_DELAY))
    {
        if (cJSON_IsString(mode) && (strcmp(mode->valuestring, "CC") == 0)) {
            current_mode = CC_MODE;
        } else if (cJSON_IsString(mode) && (strcmp(mode->valuestring, "CV") == 0)) {
            current_mode = CV_MODE;
        } else if (cJSON_IsString(mode) && (strcmp(mode->valuestring, "CP") == 0)) {
            current_mode = CP_MODE;
        }

        if (cJSON_IsNumber(current)) {
            set_current = current->valuedouble;
        }
        if (cJSON_IsNumber(voltage)) {
            set_voltage = voltage->valuedouble;
        }
        if (cJSON_IsNumber(power)) {
            set_power = power->valuedouble;
        }

        xSemaphoreGive(mode_mutex);
    }

    cJSON_Delete(json);
    httpd_resp_send(req, "Mode and parameters set", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// HTTP GET handler for serving the HTML page
esp_err_t index_get_handler(httpd_req_t *req)
{
    extern const unsigned char index_html_start[] asm("_binary_index_html_start");
    extern const unsigned char index_html_end[] asm("_binary_index_html_end");
    const size_t index_html_size = (index_html_end - index_html_start);

    httpd_resp_send(req, (const char *)index_html_start, index_html_size);
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
        httpd_uri_t set_mode_uri = {
            .uri       = "/set_mode",
            .method    = HTTP_POST,
            .handler   = set_mode_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &set_mode_uri);

        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &index_uri);
    }
    return server;
}*/