#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"

/**
 * @file http_server.h
 * @brief Header file for the HTTP server module.
 *
 * This file contains the declaration of the HTTP server initialization function
 * and includes necessary dependencies. The HTTP server provides endpoints for
 * interacting with the ESP32, such as retrieving measurements, updating setpoints,
 * and configuring safety parameters.
 *
 *
 * @date 2025-05-12
 */

/**
 * @brief Starts the HTTP server.
 *
 * Initializes and starts the HTTP server, registering URI handlers for various
 * endpoints such as `/measurement`, `/setpoint`, `/startstop`, and `/safety`.
 *
 * @param data Pointer to the measurement data structure to be used by the server.
 * @return Handle to the HTTP server instance.
 */
httpd_handle_t start_webserver();

#endif // HTTP_SERVER_H