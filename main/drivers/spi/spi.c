#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include <inttypes.h>

/**
 * @file SPI.c
 * @brief Functions for basic SPI functionaility.
 * The file contains the source code for SPI initilisation, device addition and data transmission functions.
 * @date 2025-04-15
 */


#define SPI_TAG "SPI"

extern spi_device_handle_t spi_handle;
void spi_init(gpio_num_t SCLK, gpio_num_t MOSI, gpio_num_t MISO, spi_host_device_t SPI_HOST)
{
    spi_bus_config_t bus_config = {
        .sclk_io_num = SCLK,
        .mosi_io_num = MOSI,
        .miso_io_num = MISO,
        .quadwp_io_num = -1, // Turn off Write Protect
        .quadhd_io_num = -1, // Turn off Hold
        .flags = 0,
    };

    // To check if initializing of SPI was sucsessful
    esp_err_t check_spi_init = spi_bus_initialize(SPI_HOST, &bus_config, SPI_DMA_CH_AUTO);
    ESP_LOGI(SPI_TAG, "SPI init: %s", esp_err_to_name(check_spi_init));
    ESP_ERROR_CHECK(check_spi_init);

    vTaskDelay(pdMS_TO_TICKS(100));
}

void spi_add_device(int clk_speed, int duty_val, int spi_mode, int spi_queue_size, gpio_num_t CS, spi_host_device_t SPI_HOST)
{
    spi_device_interface_config_t device_config = {
        .clock_speed_hz = clk_speed,  
        .duty_cycle_pos = duty_val,  
        .mode = spi_mode,             
        .queue_size = spi_queue_size, 
        .spics_io_num = CS,           
    };

    esp_err_t check_spi_add_device = spi_bus_add_device(SPI_HOST, &device_config, &spi_handle);
    ESP_LOGI(SPI_TAG, "SPI add device: %s", esp_err_to_name(check_spi_add_device));
    ESP_ERROR_CHECK(check_spi_add_device); // Stops the program if the function fails

    vTaskDelay(pdMS_TO_TICKS(100));
}

void spi_write_8(uint32_t address, uint8_t data)
{
    uint8_t tx_buf[5]; 

    tx_buf[0] = 0x41;                   // WRITE command (0x41)
    tx_buf[1] = (address >> 16) & 0xFF; 
    tx_buf[2] = (address >> 8) & 0xFF;  
    tx_buf[3] = address & 0xFF;         
    tx_buf[4] = data;                  

    spi_transaction_t trans = {
        .length = 5 * 8, 
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};

    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    printf("SPI  8 bit Write, Address: 0x%02X%02X%02X, Data: 0x%02X\n", tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[4]);
}

void spi_write_16(uint32_t address, uint16_t data)
{
    uint8_t tx_buf[6];

    tx_buf[0] = 0x41;                   // WRITE command (0x41)
    tx_buf[1] = (address >> 16) & 0xFF; 
    tx_buf[2] = (address >> 8) & 0xFF;  
    tx_buf[3] = address & 0xFF;        
    tx_buf[4] = data & 0xFF;            
    tx_buf[5] = (data >> 8) & 0xFF;    

    spi_transaction_t trans = {
        .length = 6 * 8,
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};

    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    printf("SPI 16 bit Write, Address: 0x%02X%02X%02X, Data: 0x%02X%02X\n", tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[5], tx_buf[4]);
}

void spi_write_32(uint32_t address, uint32_t data)
{
    
    uint8_t tx_buf[8];

    tx_buf[0] = 0x41;                   // WRITE command (0x41)              
    tx_buf[1] = (address >> 16) & 0xFF;
    tx_buf[2] = (address >> 8) & 0xFF; 
    tx_buf[3] = address & 0xFF;        
    tx_buf[4] = data & 0xFF;           
    tx_buf[5] = (data >> 8) & 0xFF;     
    tx_buf[6] = (data >> 16) & 0xFF;    
    tx_buf[7] = (data >> 24) & 0xFF;    

    spi_transaction_t trans = {
        .length = 8 * 8,
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};


    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    printf("SPI 32 bit Write, Address: 0x%02X%02X%02X, Data: 0x%02X%02X%02X%02X\n", tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[7], tx_buf[6], tx_buf[5], tx_buf[4]);
}

uint8_t spi_read_8(uint32_t address) {
    uint8_t tx_data[4] = {0};
    uint8_t rx_data[4] = {0};
    uint8_t data = 0;

    tx_data[0] = (address >> 16) & 0x3F;
    tx_data[1] = (address >> 8) & 0xFF;
    tx_data[2] = address & 0xFF;
    tx_data[3] = 0x00;

    spi_transaction_t t = {
        .length = 4 * 8,
        .rxlength = 4 * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
    };

    esp_err_t check_spi_read_8 = spi_device_transmit(spi_handle, &t);
    ESP_ERROR_CHECK(check_spi_read_8);

    data = rx_data[3];

    return data;
}

uint16_t spi_read_16(uint32_t address) {
    uint8_t tx_data[4] = {0};
    uint8_t rx_data[4] = {0};
    uint16_t data = 0;

    // Prepare the read command for FT812
    tx_data[0] = (address >> 16) & 0x3F;
    tx_data[1] = (address >> 8) & 0xFF;
    tx_data[2] = address & 0xFF;
    tx_data[3] = 0x00;

    spi_transaction_t t = {
        .length = 4 * 8,
        .rxlength = 4 * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
    };

    esp_err_t check_spi_read_16 = spi_device_transmit(spi_handle, &t);
    ESP_ERROR_CHECK(check_spi_read_16);

    // Combine two bytes into 16-bit result
    data = (rx_data[3] << 8) | rx_data[2];

    return data;
}


uint32_t spi_read_32(uint32_t address) {
    uint8_t tx_data[4] = {0};
    uint8_t rx_data[4] = {0}; 
    uint32_t data = 0;

    // Prepare the read command for FT812
    tx_data[0] = (address >> 16) & 0x3F; 
    tx_data[1] = (address >> 8) & 0xFF;  
    tx_data[2] = address & 0xFF;         
    tx_data[3] = 0x00;                  

    spi_transaction_t t = {
        .length = 4 * 8,          
        .rxlength = 4 * 8,        
        .tx_buffer = tx_data,    
        .rx_buffer = rx_data,     
    };

    esp_err_t check_spi_read_32 = spi_device_transmit(spi_handle, &t);
    ESP_ERROR_CHECK(check_spi_read_32);


    data = (rx_data[3] << 24) | (rx_data[2] << 16) | (rx_data[1] << 8) | rx_data[0];

    return data;
}
