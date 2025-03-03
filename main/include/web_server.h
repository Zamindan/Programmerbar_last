#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"

// Shared variables for mode and parameters
typedef enum { CC_MODE, CV_MODE, CP_MODE } mode_t;
extern mode_t current_mode;
extern float set_current;
extern float set_voltage;
extern float set_power;
extern SemaphoreHandle_t mode_mutex;

// Function to start the web server
httpd_handle_t start_webserver(void);

#endif // WEB_SERVER_H