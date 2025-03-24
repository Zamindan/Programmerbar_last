#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

/* Defining pins for ESP32 which uses MISO, MOSI, CS, SCLK */
#define SPI_HOST SPI2_HOST
#define SPI_DMA_CH SPI_DMA_CH_AUTO
#define SPI_TAG "SPI"

#define FSPI_SCLK 12
#define FSPI_CS 10
#define FSPI_MISO 13
#define FSPI_MOSI 11
#define FSPI_PD_N 9  // Power-down pin

void spi_init(spi_host_device_t SPI_HOST_id, gpio_num_t SPI_SCLK, gpio_num_t SPI_MOSI, gpio_num_t SPI_MISO, int transfer_size, spi_dma_chan_t DMA_CHAN)
{
    spi_bus_config_t bus_config = {
        .sclk_io_num = SPI_SCLK,
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .flags = 0,
        .max_transfer_sz = transfer_size};

    // Initialize SPI bus
    esp_err_t check_spi_init = spi_bus_initialize(SPI_HOST_id, &bus_config, DMA_CHAN);
    ESP_ERROR_CHECK(check_spi_init);
}

void spi_add_device(spi_host_device_t SPI_HOST_id, int clk_speed, int duty_val, int SPI_queue_size, gpio_num_t SPI_CS, spi_device_handle_t *handle_name)
{
    spi_device_interface_config_t device_configuration = {
        .clock_speed_hz = clk_speed,
        .duty_cycle_pos = duty_val,
        .mode = 0,  // SPI Mode 0
        .queue_size = SPI_queue_size,
        .spics_io_num = SPI_CS,
    };

    // Add SPI device
    esp_err_t check_spi_add_device = spi_bus_add_device(SPI_HOST_id, &device_configuration, handle_name);
    ESP_ERROR_CHECK(check_spi_add_device);
}

void spi_write_data(uint32_t reg_address, uint32_t data, spi_device_handle_t *handle_name)
{
    uint8_t tx_buffer[7];

    // Address (3 bytes) - LSB must have Bit 0 as 0 (for write)
    tx_buffer[0] = (reg_address >> 16) & 0xFF;  // MSB
    tx_buffer[1] = (reg_address >> 8) & 0xFF;
    tx_buffer[2] = (reg_address & 0xFE);  // LSB with Bit 0 cleared

    // Data (4 bytes)
    tx_buffer[3] = (data >> 24) & 0xFF;  // MSB
    tx_buffer[4] = (data >> 16) & 0xFF;
    tx_buffer[5] = (data >> 8) & 0xFF;
    tx_buffer[6] = data & 0xFF;          // LSB

    spi_transaction_t write_transaction = {
        .length = 8 * 7,  // 7 bytes = 56 bits
        .tx_buffer = tx_buffer};

    esp_err_t check_spi_write = spi_device_polling_transmit(*handle_name, &write_transaction);
    ESP_ERROR_CHECK(check_spi_write);
}

void spi_read_data(uint32_t reg_address, spi_device_handle_t *handle_name)
{
    uint8_t tx_buffer[4];
    uint8_t rx_buffer[5] = {0};  // Dummy + 4 bytes data

    // Address (3 bytes) - LSB must have Bit 0 as 1 (for read)
    tx_buffer[0] = (reg_address >> 16) & 0xFF;  // MSB
    tx_buffer[1] = (reg_address >> 8) & 0xFF;
    tx_buffer[2] = (reg_address | 0x01);  // LSB with Bit 0 set to 1

    // Dummy byte (required before reading data)
    tx_buffer[3] = 0x00;

    spi_transaction_t read_transaction = {
        .flags = 0,  // No special flags
        .tx_buffer = tx_buffer,
        .rx_buffer = rx_buffer,
        .length = 8 * 4,    // 4 bytes output (address + dummy)
        .rxlength = 8 * 5,   // Read 5 bytes (dummy + 4 bytes data)
    };

    esp_err_t check_spi_read = spi_device_polling_transmit(*handle_name, &read_transaction);
    ESP_ERROR_CHECK(check_spi_read);

    uint32_t data = (rx_buffer[1] << 24) | (rx_buffer[2] << 16) | (rx_buffer[3] << 8) | rx_buffer[4];
    printf("Read from 0x%06X: Data = 0x%08X\n", reg_address, data);
}

void ft812q_init(spi_device_handle_t *handle_name)
{
    // Power down and up the FT812Q
    gpio_set_direction(FSPI_PD_N, GPIO_MODE_OUTPUT);
    gpio_set_level(FSPI_PD_N, 0);  // Power down
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(FSPI_PD_N, 1);  // Power up
    vTaskDelay(pdMS_TO_TICKS(100));

    // Initialize FT812Q registers (example)
    spi_write_data(0x102400, 0x00000000, handle_name);  // Set display resolution
    spi_write_data(0x102404, 0x00000000, handle_name);  // Set touch configuration
}

int app_main(void)
{
    spi_device_handle_t SPI_handler;

    // Initialize SPI and FT812Q
    spi_init(SPI_HOST, FSPI_SCLK, FSPI_MOSI, FSPI_MISO, 4096, SPI_DMA_CH);
    spi_add_device(SPI_HOST, 30 * 1000 * 1000, 0, 1, FSPI_CS, &SPI_handler);
    ft812q_init(&SPI_handler);

    // Test read/write
    uint32_t register_address = 0x102400;  // Example register
    uint32_t data = 0x00000001;           // Example data

    spi_read_data(register_address, &SPI_handler);
    spi_write_data(register_address, data, &SPI_handler);
    spi_read_data(register_address, &SPI_handler);

    return 0;
}