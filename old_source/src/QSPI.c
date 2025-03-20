
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

/**
 * @file QSPI.c
 * @brief Functions for QSPI functionaility.
 *
 * The purpose of this document is to make basic functions for SPI functionality
 * The following code will be esential for programming the screen
 * The file contains the source code for SPI initilisation, device addition and data transmission functions.
 *
 * @author Ardit Uka
 * @date 2025-03-09
 */

#define SPI_TAG "SPI"

/**
 * @brief
 *
 * @param SPI_host_id Select SPI type
 * @param SPI_data0 Select clock GPIO for QSPI
 * @param SPI_data1 Select data1 GPIO for QSPI
 * @param SPI_data2 Select data2 GPIO for QPI
 * @param SPI_data3 Select data3 GPIO for QSPI
 * @param SPI_SCLK Select clock GPIO for QSPI
 * @param DMA_CH Select DMA chennel for QSPI
 *
 * @details This function gives the ability to choose which SPI to use
 * @details This functoon gives the ability to choose which SPI GPIO to use
 * @details This function gives the ability to select how the DMA is going to opperate.
 * @details ERROR checing the initializing of SPI
 */
void SPI_init(spi_host_device_t SPI_HOST_id, gpio_num_t SPI_data0, gpio_num_t SPI_data1, gpio_num_t SPI_data2, gpio_num_t SPI_data3, gpio_num_t SPI_SCLK, spi_dma_chan_t DMA_CHAN)
{
    spi_bus_config_t bus_config = {
        .data0_io_num = SPI_data0,
        .data1_io_num = SPI_data1,
        .data2_io_num = SPI_data2,
        .data3_io_num = SPI_data3, 
        .sclk_io_num = SPI_SCLK,
        .mosi_io_num = -1,   // Deactivate MOSI functionality
        .miso_io_num = -1,   // Deactivate MISO functionality
        .quadwp_io_num = -1, // Deactivate write protect functionality
        .quadhd_io_num = -1, // Deactivate hold functionality
        .flags = SPI_TRANS_MODE_QIO | SPI_TRANS_MULTILINE_ADDR,  // flag for QSPI inn I/O mode 
    };

    // Initialize SPI
    esp_err_t check_SPI_init = spi_bus_initialize(SPI_HOST_id, &bus_config, DMA_CHAN);

    // Check if SPI initialize was sucsessful
    ESP_LOGE(SPI_TAG, esp_err_to_name(check_SPI_init));

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
void SPI_add_device(spi_host_device_t SPI_HOST_id, int clk_speed, int duty_val, int SPI_queue_size, gpio_num_t SPI_CS, spi_device_handle_t *handle_name)
{
    spi_device_interface_config_t device_configuration = {
        .clock_speed_hz = clk_speed,
        .duty_cycle_pos = duty_val,
        .mode = 0,
        .queue_size = SPI_queue_size,
        .spics_io_num = SPI_CS,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .command_bits = 1,
        .address_bits = 4,
        .dummy_bits = 1, 
    };

    // Chech if adding a new SPI device was sucsessful
    esp_err_t check_spi_add_device = spi_bus_add_device(SPI_HOST_id, &device_configuration, handle_name);
    ESP_LOGE(SPI_TAG, esp_err_to_name(check_spi_add_device));
    vTaskDelay(pdMS_TO_TICKS(10));
}

//////////////////////////// The other functions differs more than the two above, and are therefor not included ////////////////////////////////////////