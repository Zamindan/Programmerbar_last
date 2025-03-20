#include "web_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "measurements.h"
#include "control.h"

static const char *TAG = "WEB_SERVER";

esp_err_t index_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html, index_html_len);
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

    }
    else
    {
        ESP_LOGI(TAG, "Server failed to start");
    };
    return server;
}