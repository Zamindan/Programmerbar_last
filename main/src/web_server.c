#include "web_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "web_interface.h"
#include "measurements.h"
#include "control.h"

static const char *TAG = "WEB_SERVER";

extern struct measurement_data measurements;
extern struct control_data control_data;

extern SemaphoreHandle_t sensor_mutex;


esp_err_t index_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html, index_html_len);
    return ESP_OK;
}

esp_err_t measurements_handler(httpd_req_t *req) {
    if(xSemaphoreTake(sensor_mutex, pdMS_TO_TICKS(100))) {
    

    char json_buf[128];
    snprintf(json_buf, sizeof(json_buf),
        "{\"voltage\":%.2f,\"current\":%.2f,\"temperature\":%.1f}",
        measurements.voltage, measurements.current, measurements.temperature);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_buf, HTTPD_RESP_USE_STRLEN);
    xSemaphoreGive(sensor_mutex);
    } else {
        ESP_LOGW(TAG, "Failed to send sensor data - mutex timeout");
    }
    return ESP_OK;
}

esp_err_t emergency_handler(httpd_req_t *req) {
    httpd_resp_sendstr(req, "Emergency shutdown activated");
    return ESP_OK;
}

httpd_handle_t start_webserver(struct measurement_data* data)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    esp_err_t test = httpd_start(&server, &config);

    if (test == ESP_OK)
    {
        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &index_uri);

        /*httpd_uri_t controls_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = measurements_handler,
            .user_ctx  = NULL
        };*/

        httpd_uri_t measurments_uri = {
            .uri       = "/measurements",
            .method    = HTTP_GET,
            .handler   = measurements_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &measurments_uri);

        httpd_uri_t emergency_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = emergency_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &emergency_uri);

    }
    else
    {
        ESP_LOGI(TAG, "Server failed to start");
    };
    return server;
}

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