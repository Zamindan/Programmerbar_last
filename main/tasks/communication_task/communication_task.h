#ifndef COMMUNICATION_TASK_H_
#define COMMUNICATION_TASK_H_

#include "esp_err.h"

// Define constants for WiFi status
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1

void communication_task(void *pvParameters);

// Function to connect to WiFi
esp_err_t wifi_init();

void wifi_start();

#endif