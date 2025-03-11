#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"

// Define constants for WiFi status
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1

// Function to connect to WiFi
esp_err_t connect_wifi();

void wifi_start();

#endif // WIFI_H