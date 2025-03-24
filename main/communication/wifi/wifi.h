#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"

/**
 * @brief Handler for configuring safety parameters.
 *
 * This handler responds to POST requests to the `/safety` endpoint by
 * updating the safety parameters based on the received JSON data.
 *
 * @param req Pointer to the HTTP request.
 * @return ESP_OK on success, or an error code on failure.
 */

/**
 * @brief Connects to a WiFi network.
 *
 * This function initializes the WiFi driver, sets up the WiFi station mode,
 * and attempts to connect to the configured access point (AP). It waits for
 * either a successful connection or a failure event.
 *
 * @return
 * - `ESP_OK` if the connection is successful.
 * - `ESP_FAIL` if the connection fails.
 */
esp_err_t connect_wifi();

/**
 * @brief Starts the WiFi module.
 *
 * This function initializes the non-volatile storage (NVS) and starts the WiFi
 * connection process by calling `connect_wifi()`. If the connection fails, it
 * logs an error and stops further execution.
 */
void wifi_start();

#endif // WIFI_H