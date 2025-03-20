#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "web_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"

extern struct measurement_data simulated_data;

// Function to start the web server
httpd_handle_t start_webserver(struct measurement_data* data);

#endif // WEB_SERVER_H