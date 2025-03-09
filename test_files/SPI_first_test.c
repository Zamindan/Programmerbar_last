
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

/**
 * @file SPI.c
 * @brief Functions for SPI functionaility.
 *
 * The purpose of this document is to make basic functions for SPI functionality
 * The following code will be esential for programming the screen
 * The file contains the source code for SPI initilisation, device addition and data transmission functions.
 *
 * @author Ardit Uka
 * @date 2025-03-03
 */

#define SPI_HOST SPI2_HOST
#define FSPI_DMA_CH SPI_DMA_CH_AUTO
#define SPI_TAG "SPI"

#define FSPI_SCLK 12
#define FSPI_CS 10
#define FSPI_MISO 13
#define FSPI_MOSI 11

void SPI_init(gpio_num_t SPI_SCLK, gpio_num_t SPI_MOSI, gpio_num_t SPI_MISO, spi_dma_chan_t DMA_CH)
{
    spi_bus_config_t bus_config = {
        .sclk_io_num = SPI_SCLK,
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .quadwp_io_num = -1, // Turn off Write Protect
        .quadhd_io_num = -1, // Ruen ff Hold ()
        .flags = 0,          // Viktig for nyere ESP-IDF versjoner
    };

    // To check if initializing of SPI was sucsessful
    esp_err_t check_SPI_init = spi_bus_initialize(SPI_HOST, &bus_config, DMA_CH);
    if (check_SPI_init != ESP_OK)
    {
        ESP_LOGE(SPI_TAG, "SPI initialization failed: %s", esp_err_to_name(check_SPI_init));
    }
    if (check_SPI_init == ESP_OK)
    {
        ESP_LOGE(SPI_TAG, "SPI initialization success: %s", esp_err_to_name(check_SPI_init));
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}

void SPI_add_device(int clk_speed, int duty_val, int SPI_mode, int SPI_queue_size, gpio_num_t SPI_CS, spi_device_handle_t *handle_name)
{
    spi_device_interface_config_t device_configuration = {
        .clock_speed_hz = clk_speed,  // Velg klokkehastighet
        .duty_cycle_pos = duty_val,   //
        .mode = SPI_mode,             // CPOL og CPHA (0 - 3)
        .queue_size = SPI_queue_size, // Definer køstørrelse
        .spics_io_num = SPI_CS,       // Velg CS-pin
    };

    esp_err_t check_spi_add_device = spi_bus_add_device(SPI_HOST, &device_configuration, handle_name);
    if (check_spi_add_device != ESP_OK)
    {
        ESP_LOGE(SPI_TAG, "SPI add device failed: %s", esp_err_to_name(check_spi_add_device));
    }

    if (check_spi_add_device == ESP_OK)
    {
        ESP_LOGE(SPI_TAG, "SPI add device success: %s", esp_err_to_name(check_spi_add_device));
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}


void app_main(){

    spi_device_handle_t SPI_handler;

    SPI_init(FSPI_SCLK, FSPI_MOSI, FSPI_MISO, FSPI_DMA_CH);
    SPI_add_device(1000000, 128, 0, 1, FSPI_CS, &SPI_handler);

    char TX_buf[128] = {0};
    char RX_buf[128] = {0};

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    while (1)
    {
        snprintf(TX_buf, sizeof(TX_buf), "Testing Fast SPI for ESP32");
        t.length = sizeof(TX_buf) * 8;
        t.tx_buffer = TX_buf;
        t.rx_buffer = RX_buf;
        spi_device_transmit(SPI_handler, &t);
        printf("Message sendt from MOSI: %s\n", TX_buf);
        printf("Message received from MISO: %s\n", RX_buf);
        vTaskDelay(pdMS_TO_TICKS(1000));
    };
}