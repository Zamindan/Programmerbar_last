
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

#define SPI_TAG "SPI"

/**
 * @brief
 *
 * @param SPI_host_id
 * @param SPI_SCLK Select clock GPIO for SPI
 * @param SPI_MOSI Select MOSI GPIO for SPI
 * @param SPI_MISO Select MISO GPIO for SPI
 * @param DMA_CH Select DMA chennel for SPI
 *
 * @details This function gives the ability to choose which SPI to use
 * @details This functoon gives the ability to choose which SPI GPIO to use
 * @details This function gives the ability to select how the DMA is going to opperate.
 * @details ERROR checing the initializing of SPI
 */
void spi_init(spi_host_device_t SPI_HOST_id, gpio_num_t SPI_SCLK, gpio_num_t SPI_MOSI, gpio_num_t SPI_MISO, spi_dma_chan_t DMA_CHAN)
{
    spi_bus_config_t bus_config = {
        .sclk_io_num = SPI_SCLK,
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .quadwp_io_num = -1, // Deactivate write protect functionality
        .quadhd_io_num = -1, // Deactivate hold functionality
        .flags = 0,          // Choosing flag for standard SPI
    };

    // Check if initializing of SPI was sucsessful
    esp_err_t check_SPI_init = spi_bus_initialize(SPI_HOST_id, &bus_config, DMA_CHAN);
    ESP_LOGE(SPI_TAG, "Initialize: %s", esp_err_to_name(check_SPI_init));
    vTaskDelay(pdMS_TO_TICKS(10)); // Delay
}

/**
 * @brief Function for adding SPI devices
 *
 * @param SPI_HOST_id Select which SPI to use
 * @param clk_speed Set clock speed for SPI
 * @param duty_val Scale down the clock to match the communication speed
 * @param SPI_queue_size Choosing how many operation can occure at once
 * @param SPI_CS Select CS GPIO
 * @param hande_name Choose a handler for SPI operation
 *
 */
void spi_add_device(spi_host_device_t SPI_HOST_id, int clk_speed, int duty_val, int SPI_queue_size, gpio_num_t SPI_CS, spi_device_handle_t *handle_name)
{

    spi_device_interface_config_t device_configuration = {
        .clock_speed_hz = clk_speed,
        .duty_cycle_pos = duty_val,
        .mode = 0,
        .queue_size = SPI_queue_size,
        .spics_io_num = SPI_CS,
    };

    // Chech if adding a new SPI device was sucsessful
    esp_err_t check_spi_add_device = spi_bus_add_device(SPI_HOST_id, &device_configuration, handle_name);
    if (check_spi_add_device =  )
    ESP_LOGE(SPI_TAG, "Add device: %s", esp_err_to_name(check_spi_add_device));
    vTaskDelay(pdMS_TO_TICKS(10));
}