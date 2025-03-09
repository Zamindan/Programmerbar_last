
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

#define FSPI_SCLK GPIO_NUM_12
#define FSPI_CS GPIO_NUM_10
#define FSPI_MISO GPIO_NUM_13
#define FSPI_MOSI GPIO_NUM_11

void SPI_init(spi_host_device_t SPI_host_id ,gpio_num_t SPI_SCLK, gpio_num_t SPI_MOSI, gpio_num_t SPI_MISO, spi_dma_chan_t DMA_CH)
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
    esp_err_t check_SPI_init = spi_bus_initialize(SPI_host_id, &bus_config, DMA_CH);
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

void SPI_add_device(int clk_speed, int duty_val, int SPI_queue_size, gpio_num_t SPI_CS, spi_device_handle_t *handle_name)
{
    spi_device_interface_config_t device_configuration = {
        .clock_speed_hz = clk_speed,  // Velg klokkehastighet
        .duty_cycle_pos = duty_val,   //
        .mode = 0,         
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

void SPI_write(spi_device_handle_t spi_write_handle, uint8_t register_address, uint16_t data_to_write)
{

    spi_transaction_t spi_transaction;
    memset(&spi_transaction, 0, sizeof(spi_transaction));

    uint8_t TX_buf[3];
    TX_buf[0] = register_address | 0x80;     // Ensure MSB is set for write if required
    TX_buf[1] = (data_to_write >> 8) & 0xFF; // High byte
    TX_buf[2] = data_to_write & 0xFF;        // Low byte

    spi_transaction.length = sizeof(TX_buf) * 8;
    spi_transaction.tx_buffer = TX_buf;
    spi_transaction.flags = 0;

    // Perform SPI transmission
    esp_err_t check_spi_write = spi_device_transmit(spi_write_handle, &spi_transaction);
    if (check_spi_write != ESP_OK)
    {
        ESP_LOGE(SPI_TAG, "SPI Write Failed: %s", esp_err_to_name(check_spi_write));
        return;
    }
    if (check_spi_write == ESP_OK)
    {
        ESP_LOGE(SPI_TAG, "SPI Write success: %s", esp_err_to_name(check_spi_write));
        return;
    }

    ESP_LOGI(SPI_TAG, "Wrote 0x%04X to register 0x%02X", data_to_write, register_address);
}

float SPI_read(spi_device_handle_t spi_read_handle, uint8_t register_address)
{
    uint8_t TX_buf[2];  // {register_address & 0x7F, 0x00};  // MSB=0 for lesing
    uint8_t RX_buf[2] = {0};

    spi_transaction_t SPI_read_trans = {
        .length = 8 * sizeof(TX_buf),
        .tx_buffer = TX_buf,
        .rxlength = 8 * sizeof(RX_buf), // Length in bits for receiving
        .rx_buffer = RX_buf,
        .flags = 0};

    esp_err_t check_SPI_read = spi_device_transmit(spi_read_handle, &SPI_read_trans);
    if (check_SPI_read != ESP_OK)
    {
        ESP_LOGE(SPI_TAG, "SPI transmission failed: %s", esp_err_to_name(check_SPI_read));
    }

    // Combine the read data into a single value
    uint16_t combined_data = (RX_buf[0] << 8) | RX_buf[1];
    float result = (float)combined_data;
    ESP_LOGI(SPI_TAG, "Read data from register 0x%02X: 0x%04X", register_address, combined_data);
    return result;
}

void app_main()
{
    spi_device_handle_t SPI_handler;

    SPI_init(FSPI_SCLK, FSPI_MOSI, FSPI_MISO, FSPI_DMA_CH);
    SPI_add_device(1000000, 0, 1, FSPI_CS, &SPI_handler);

    while (1)
    {
        SPI_write(SPI_handler, 0x02, 0x1234);
        SPI_read(SPI_handler, 0x05); 
        ESP_LOGI(SPI_TAG, "Running main loop...");
        vTaskDelay(pdMS_TO_TICKS(1000)); // Unngå at watchdog-timeren utløses
    }
}

/*

esp_err_t spi_transfer(spi_device_handle_t spi, uint8_t *tx_data, uint8_t *rx_data, size_t length) {
    spi_transaction_t trans = {
        .length = length * 8, // Length in bits
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };

    esp_err_t ret = spi_device_transmit(spi, &trans);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SPI transfer successful");
    } else {
        ESP_LOGE(TAG, "SPI transfer failed");
    }
    return ret;
}

esp_err_t spi_write_register(spi_device_handle_t spi, uint8_t reg, uint8_t value) {
    uint8_t tx_data[2] = { reg, value };
    uint8_t rx_data[2] = { 0 };
    return spi_transfer(spi, tx_data, rx_data, 2);
}

esp_err_t spi_read_register(spi_device_handle_t spi, uint8_t reg, uint8_t *value) {
    uint8_t tx_data[2] = { reg | 0x80, 0x00 };  // Assuming 0x80 sets read mode
    uint8_t rx_data[2] = { 0 };
    esp_err_t ret = spi_transfer(spi, tx_data, rx_data, 2);
    if (ret == ESP_OK) {
        *value = rx_data[1];
        ESP_LOGI(TAG, "Read register 0x%X: 0x%X", reg, *value);
    }
    return ret;
}


*/