#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "hmi_task.h"
#include "measurement_task.h"
#include "safety_task.h"
#include "globals.h"
#include "config.h"

#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"

#include "esp_http_server.h"
#include "esp_log.h"

#include "cJSON.h"


/**
 * @file http_server.c
 * @brief Implementation of the HTTP server module.
 *
 * This file contains the implementation of the HTTP server, which provides
 * endpoints for interacting with the ESP32. The server handles requests for
 * retrieving measurements, updating setpoints, starting/stopping the load,
 * and configuring safety parameters.
 *
 * @author Sondre
 * @date 2025-03-24
 */



static const char *TAG = "SERVER"; /**< Tag for logging messages from the HTTP server module. */

/**
 * @brief HTML content for the main page.
 *
 * This HTML page displays the current measurements and allows the user to
 * update the setpoint.
 */
static const char *index_html =
"<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"    <meta charset=\"UTF-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"    <title>Programmable Load Control</title>"
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
"            background-color: #1a1a1a;"
"            color: #fff;"
"        }"
"        .container {"
"            background: #2a2a2a;"
"            padding: 20px;"
"            border-radius: 8px;"
"            box-shadow: 0 0 15px rgba(0, 0, 0, 0.5);"
"            width: 90%;"
"            max-width: 600px;"
"        }"
"        h1 {"
"            text-align: center;"
"        }"
"        .measurement, .setpoint, .safety-limits, .startstop {"
"            margin-bottom: 20px;"
"        }"
"        .measurement span {"
"            color: #4CAF50;"
"            font-weight: bold;"
"        }"
"        input[type=\"number\"] {"
"            background-color: #333;"
"            color: white;"
"            border: 1px solid #4CAF50;"
"            padding: 8px;"
"            border-radius: 4px;"
"            margin-right: 10px;"
"            width: 100%;"
"        }"
"        button {"
"            background-color: #4CAF50;"
"            color: white;"
"            border: none;"
"            padding: 10px 20px;"
"            border-radius: 5px;"
"            cursor: pointer;"
"            transition: all 0.3s ease;"
"        }"
"        button:hover {"
"            background-color: #45a049;"
"        }"
"        button:active {"
"            transform: scale(0.95);"
"        }"
"    </style>"
"</head>"
"<body>"
"    <div class=\"container\">"
"        <h1>Programmable Load Control</h1>"
"        <div class=\"measurement\">"
"            <h2>Measurements</h2>"
"            <p><strong>Voltage:</strong> <span id=\"voltage\">0.0000</span> V</p>"
"            <p><strong>Current:</strong> <span id=\"current\">0.0000</span> A</p>"
"            <p><strong>Power:</strong> <span id=\"power\">0.0000</span> W</p>"
"            <p><strong>Internal Temperature:</strong> <span id=\"temperature_internal\">0.0000</span> °C</p>"
"            <p><strong>External Temperature 1:</strong> <span id=\"temperature_external_1\">0.0000</span> °C</p>"
"            <p><strong>External Temperature 2:</strong> <span id=\"temperature_external_2\">0.0000</span> °C</p>"
"            <p><strong>External Temperature 3:</strong> <span id=\"temperature_external_3\">0.0000</span> °C</p>"
"        </div>"
"        <div class=\"setpoint\">"
"            <h2>Setpoint</h2>"
"            <input type=\"number\" id=\"setpoint\" step=\"0.01\" placeholder=\"Enter setpoint\">"
"            <button onclick=\"updateSetpoint()\">Update Setpoint</button>"
"        </div>"
"        <div class=\"startstop\">"
"            <h2>Start/Stop</h2>"
"            <button onclick=\"toggleStartStop(this)\">Start</button>"
"        </div>"
"        <div class=\"safety-limits\">"
"            <h2>Safety Limits</h2>"
"            <h3>Hard Limits</h3>"
"            <input type=\"number\" id=\"max_voltage_user\" placeholder=\"Max Voltage (V)\">"
"            <input type=\"number\" id=\"min_voltage_user\" placeholder=\"Min Voltage (V)\">"
"            <input type=\"number\" id=\"max_current_user\" placeholder=\"Max Current (A)\">"
"            <input type=\"number\" id=\"max_power_user\" placeholder=\"Max Power (W)\">"
"            <input type=\"number\" id=\"max_temperature_user\" placeholder=\"Max Temperature (°C)\">"
"            <h3>Soft Limits</h3>"
"            <input type=\"number\" id=\"soft_max_voltage\" placeholder=\"Soft Max Voltage (V)\">"
"            <input type=\"number\" id=\"soft_max_current\" placeholder=\"Soft Max Current (A)\">"
"            <input type=\"number\" id=\"soft_max_temperature\" placeholder=\"Soft Max Temperature (°C)\">"
"            <button onclick=\"updateSafetyLimits()\">Set Safety Limits</button>"
"        </div>"
"    </div>"
"    <script>"
"        async function fetchMeasurements() {"
"            const response = await fetch('/measurement');"
"            const data = await response.json();"
"            document.getElementById('voltage').textContent = data.voltage.toFixed(4);"
"            document.getElementById('current').textContent = data.current.toFixed(4);"
"            document.getElementById('power').textContent = data.power.toFixed(4);"
"            document.getElementById('temperature_internal').textContent = data.temperature_internal.toFixed(4);"
"            document.getElementById('temperature_external_1').textContent = data.temperature_external_1.toFixed(4);"
"            document.getElementById('temperature_external_2').textContent = data.temperature_external_2.toFixed(4);"
"            document.getElementById('temperature_external_3').textContent = data.temperature_external_3.toFixed(4);"
"        }"
"        async function updateSetpoint() {"
"            const setpoint = document.getElementById('setpoint').value;"
"            await fetch('/setpoint', {"
"                method: 'POST',"
"                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },"
"                body: setpoint,"
"            });"
"        }"
"        async function toggleStartStop(button) {"
"            const action = button.textContent === \"Start\" ? \"start\" : \"stop\";"
"            await fetch('/startstop', {"
"                method: 'POST',"
"                body: action,"
"            });"
"            button.textContent = action === \"start\" ? \"Stop\" : \"Start\";"
"        }"
"        async function updateSafetyLimits() {"
"            const safetyData = {"
"                max_voltage_user: parseFloat(document.getElementById('max_voltage_user').value),"
"                min_voltage_user: parseFloat(document.getElementById('min_voltage_user').value),"
"                max_current_user: parseFloat(document.getElementById('max_current_user').value),"
"                max_power_user: parseFloat(document.getElementById('max_power_user').value),"
"                max_temperature_user: parseFloat(document.getElementById('max_temperature_user').value),"
"                soft_max_voltage: parseFloat(document.getElementById('soft_max_voltage').value),"
"                soft_max_current: parseFloat(document.getElementById('soft_max_current').value),"
"                soft_max_temperature: parseFloat(document.getElementById('soft_max_temperature').value),"
"            };"
"            await fetch('/safety', {"
"                method: 'POST',"
"                headers: { 'Content-Type': 'application/json' },"
"                body: JSON.stringify(safetyData),"
"            });"
"        }"
"        setInterval(fetchMeasurements, 1000);"
"    </script>"
"</body>"
"</html>";


/**
 * @brief Handler for serving the main HTML page.
 *
 * This handler responds to GET requests to the `/` endpoint by serving the
 * main HTML page, which displays measurements and allows the user to update
 * the setpoint.
 *
 * @param req Pointer to the HTTP request.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, strlen(index_html));
    return ESP_OK;
}

/**
 * @brief Handler for retrieving measurement data.
 *
 * This handler responds to GET requests to the `/measurement` endpoint by
 * returning the current measurement data in JSON format.
 *
 * @param req Pointer to the HTTP request.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t get_measurement_handler(httpd_req_t *req) {
    MeasurementData measurement;
    if (xQueuePeek(measurement_queue, &measurement, pdMS_TO_TICKS(10)) == pdTRUE) {
        char resp[200];
        snprintf(resp, sizeof(resp),
                 "{\"voltage\": %.4f, \"current\": %.4f, \"power\": %.4f, "
                 "\"temperature_internal\": %.2f, \"temperature_external_1\": %.2f, "
                 "\"temperature_external_2\": %.2f, \"temperature_external_3\": %.2f}",
                 measurement.bus_voltage, measurement.current, measurement.power,
                 measurement.temperature_internal, measurement.temperature_external_1,
                 measurement.temperature_external_2, measurement.temperature_external_3);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, resp, strlen(resp));
    } else {
        httpd_resp_send_500(req);
    }
    return ESP_OK;
}

/**
 * @brief Handler for updating the setpoint value.
 *
 * This handler responds to POST requests to the `/setpoint` endpoint by
 * updating the setpoint value and signaling other tasks.
 *
 * @param req Pointer to the HTTP request.
 * @return ESP_OK on success, or an error code on failure.
 */
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

    // Writes new setpoint to queue:
    xQueueOverwrite(setpoint_queue, &setpoint);

    // Sets event group bits, signals that new data is ready to be read by tasks that need setpoint.
    xEventGroupSetBits(signal_event_group, COMMUNICATION_SETPOINT_BIT);
    xEventGroupSetBits(signal_event_group, HMI_SETPOINT_BIT);
    xEventGroupSetBits(signal_event_group, CONTROL_SETPOINT_BIT);

    httpd_resp_send(req, "Setpoint updated", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/**
 * @brief Handler for starting or stopping the load.
 *
 * This handler responds to POST requests to the `/startstop` endpoint by
 * toggling the start/stop state of the load based on the received command.
 *
 * @param req Pointer to the HTTP request.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t start_stop_handler(httpd_req_t *req) {
    char content[10];
    size_t recv_size = MIN(req->content_len, sizeof(content) - 1);
    int ret = httpd_req_recv(req, content, recv_size);
    
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    content[recv_size] = '\0';

    // Toggle the event group bit based on received command
    if (strcmp(content, "start") == 0) {
        xEventGroupSetBits(signal_event_group, START_STOP_BIT);
    } else if (strcmp(content, "stop") == 0) {
        xEventGroupClearBits(signal_event_group, START_STOP_BIT);
    }

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/**
 * @brief Handler for configuring safety parameters.
 *
 * This handler responds to POST requests to the `/safety` endpoint by
 * updating the safety parameters based on the received JSON data.
 *
 * @param req Pointer to the HTTP request.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t set_safety_handler(httpd_req_t *req) {
    char content[300];
    size_t recv_size = MIN(req->content_len, sizeof(content) - 1);
    int ret = httpd_req_recv(req, content, recv_size);

    if (ret <= 0) return ESP_FAIL;
    content[recv_size] = '\0';

    cJSON *root = cJSON_Parse(content);
    if (!root) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    SafetyData safety;
    memset(&safety, 0, sizeof(safety));

    safety.max_voltage_user = cJSON_GetObjectItem(root, "max_voltage_user")->valuedouble;
    safety.min_voltage_user = cJSON_GetObjectItem(root, "min_voltage_user")->valuedouble;
    safety.max_current_user = cJSON_GetObjectItem(root, "max_current_user")->valuedouble;
    safety.max_power_user = cJSON_GetObjectItem(root, "max_power_user")->valuedouble;
    safety.max_temperature_user = cJSON_GetObjectItem(root, "max_temperature_user")->valuedouble;
    safety.soft_max_voltage = cJSON_GetObjectItem(root, "soft_max_voltage")->valuedouble;
    safety.soft_max_current = cJSON_GetObjectItem(root, "soft_max_current")->valuedouble;
    safety.soft_max_temperature = cJSON_GetObjectItem(root, "soft_max_temperature")->valuedouble;

    if (xQueueSend(safety_queue, &safety, pdMS_TO_TICKS(100)) != pdPASS) {
        cJSON_Delete(root);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    cJSON_Delete(root);
    httpd_resp_send(req, "Safety limits updated", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t set_mode_handler(httpd_req_t *req) {
    char content[20];
    size_t recv_size = MIN(req->content_len, sizeof(content) - 1);
    int ret = httpd_req_recv(req, content, recv_size);

    if (ret <= 0) return ESP_FAIL;
    content[recv_size] = '\0';

    ControlMode mode;
    if (strcmp(content, "MODE_CC") == 0) {
        mode = MODE_CC;
    } else if (strcmp(content, "MODE_CV") == 0) {
        mode = MODE_CV;
    } else if (strcmp(content, "MODE_CP") == 0) {
        mode = MODE_CP;
    } else {
        httpd_resp_send_400(req);
        return ESP_FAIL;
    }

    if (xQueueOverwrite(mode_queue, &mode) != pdPASS) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_send(req, "Mode updated", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/**
 * @brief Starts the HTTP server.
 *
 * Initializes and starts the HTTP server, registering URI handlers for
 * various endpoints such as `/`, `/measurement`, `/setpoint`, `/startstop`, and `/safety`.
 *
 * @return Handle to the HTTP server instance, or NULL on failure.
 */
httpd_handle_t start_webserver()
{
    // Create http handle and config.
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start http server with the above handle and config
    esp_err_t ret = httpd_start(&server, &config);
    if (ret == ESP_OK) {

        // Register uri's with their handler and initialise them
        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &index_uri);

        httpd_uri_t startstop_uri = {
            .uri       = "/startstop",
            .method    = HTTP_POST,
            .handler   = start_stop_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &startstop_uri);

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

        httpd_uri_t safety_uri = {
            .uri = "/safety",
            .method = HTTP_POST,
            .handler = set_safety_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &safety_uri);

        httpd_uri_t mode_uri = {
            .uri       = "/mode",
            .method    = HTTP_POST,
            .handler   = set_mode_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &mode_uri);

    } else {
        ESP_LOGI(TAG, "Server failed to start");
    }
    return server;
}