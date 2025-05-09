#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"

#include "config.h"
#include "globals.h"

/*
Code from this github:
https://github.com/lowleveltv/esp32-wifi
*/


/**
 * @file wifi.c
 * @brief Implementation of the WiFi module.
 *
 * This file contains the implementation of functions for initializing and connecting
 * to a WiFi network. It handles WiFi events, manages retries, and signals connection
 * status using FreeRTOS event groups.
 *
 * @author Sondre
 * @date 2025-03-24
 */


// Event group to contain status information
static EventGroupHandle_t wifi_event_group;

// Retry tracker
static int s_retry_num = 0;


static const char *TAG = "WIFI"; /**< Tag for logging messages from the wifi module. */

/**
 * @brief WiFi event handler.
 *
 * Handles WiFi events such as starting the station, disconnecting from the AP,
 * and retrying connections. Signals success or failure using the event group.
 *
 * @param arg Unused parameter.
 * @param event_base The base of the event (e.g., WIFI_EVENT).
 * @param event_id The ID of the event (e.g., WIFI_EVENT_STA_START).
 * @param event_data Pointer to event-specific data.
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Connecting to AP...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < MAX_FAILURES)
        {
            ESP_LOGI(TAG, "Retrying connection to AP...");
            esp_wifi_connect();
            s_retry_num++;
        }
        else
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
        }
    }
}

/**
 * @brief IP event handler.
 *
 * Handles IP events such as successfully obtaining an IP address. Signals
 * success using the event group.
 *
 * @param arg Unused parameter.
 * @param event_base The base of the event (e.g., IP_EVENT).
 * @param event_id The ID of the event (e.g., IP_EVENT_STA_GOT_IP).
 * @param event_data Pointer to event-specific data.
 */
static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
    }
}

/**
 * @brief Connects to a WiFi network.
 *
 * Initializes the WiFi driver, sets up the station mode, and attempts to connect
 * to the configured access point (AP). Waits for either a successful connection
 * or a failure event.
 *
 * @return
 * - `ESP_OK` if the connection is successful.
 * - `ESP_FAIL` if the connection fails.
 */
esp_err_t connect_wifi()
{
    int status = WIFI_FAILURE;

    // Initialise the esp network interface
    ESP_ERROR_CHECK(esp_netif_init());

    // Initialise default esp event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create wifi station in the wifi driver
    esp_netif_create_default_wifi_sta();

    // setup wifi station with the default wifi config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &wifi_handler_event_instance));

    esp_event_handler_instance_t ip_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &ip_event_handler,
                                                        NULL,
                                                        &ip_handler_event_instance));

    /// Start the WIFI driver

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    // Set the WiFi controller to be in station mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Set WiFi configuration
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Start the WiFi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA init complete");

    // Wait for the connection to be complete or fail
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_SUCCESS | WIFI_FAILURE, pdFALSE, pdFALSE, portMAX_DELAY);

    // xEvenGroupWaitBits() returns the bits before the call returned, so we test which event happened

    if (bits & WIFI_SUCCESS)
    {
        ESP_LOGI(TAG, "Connected to AP");
        status = WIFI_SUCCESS;
    }
    else if (bits & WIFI_FAILURE)
    {
        ESP_LOGI(TAG, "Failed to connect to AP");
        status = WIFI_FAILURE;
    }
    else
    {
        ESP_LOGE(TAG, "Unexpected event");
        status = WIFI_FAILURE;
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, ip_handler_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

    return status;
}

/**
 * @brief Starts the WiFi module.
 *
 * Initializes the non-volatile storage (NVS) and starts the WiFi connection
 * process by calling `connect_wifi()`. If the connection fails, it logs an
 * error and stops further execution.
 */
void wifi_start()
{
    esp_err_t status = WIFI_FAILURE;
    // initialize storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // connect to wireless AP
    status = connect_wifi();
    if (WIFI_SUCCESS != status)
    {
        ESP_LOGI(TAG, "Failed to associate to AP, dying...");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "Connected to AP");
    }
}
