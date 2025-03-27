#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

#define SPI_TAG "SPI"
#define LCD_TAG "LCD"

#define SPI_HOST SPI2_HOST // Use SPI2 (HSPI)
#define GPIO_MISO GPIO_NUM_13
#define GPIO_MOSI GPIO_NUM_11
#define GPIO_SCLK GPIO_NUM_12
#define GPIO_CS GPIO_NUM_10
#define GPIO_PD GPIO_NUM_18 // Power Down (PD#) Pin


spi_device_handle_t spi_handle;

void spi_init(gpio_num_t SCLK, gpio_num_t MOSI, gpio_num_t MISO)
{
    spi_bus_config_t bus_config = {
        .sclk_io_num = SCLK,
        .mosi_io_num = MOSI,
        .miso_io_num = MISO,
        .quadwp_io_num = -1, // Turn off Write Protect
        .quadhd_io_num = -1, // Turn off Hold
        .flags = 0,          // Important for newer ESP-IDF versions
    };

    // To check if initializing of SPI was sucsessful
    esp_err_t check_spi_init = spi_bus_initialize(SPI_HOST, &bus_config, SPI_DMA_CH_AUTO);
    ESP_LOGI(SPI_TAG, "SPI init: %s", esp_err_to_name(check_spi_init));
    ESP_ERROR_CHECK(check_spi_init);

    vTaskDelay(pdMS_TO_TICKS(100));
}

void spi_add_device(int clk_speed, int duty_val, int spi_mode, int spi_queue_size, gpio_num_t CS)
{
    spi_device_interface_config_t device_config = {
        .clock_speed_hz = clk_speed,  // Velg klokkehastighet
        .duty_cycle_pos = duty_val,   //
        .mode = spi_mode,             // CPOL og CPHA (0 - 3)
        .queue_size = spi_queue_size, // Definer køstørrelse
        .spics_io_num = CS,           // Velg CS-pin
    };

    esp_err_t check_spi_add_device = spi_bus_add_device(SPI_HOST, &device_config, &spi_handle);
    ESP_LOGI(SPI_TAG, "SPI add device: %s", esp_err_to_name(check_spi_add_device));
    ESP_ERROR_CHECK(check_spi_add_device); // Stops the program if the function fails

    vTaskDelay(pdMS_TO_TICKS(100));
}

void spi_write_8(uint32_t address, uint8_t data)
{
    // FT812 requires: [WRITE_CMD(0x41)] + [24-bit address] + [data]

    uint8_t tx_buf[5];

    tx_buf[0] = WRITE;                  // WRITE command
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = (address) & 0xFF;       // Address byte 0 (LSB)
    tx_buf[4] = data;                   // Data to write

    spi_transaction_t trans = {
        .length = 5 * 8, // Total bits to send (5 bytes)
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};

    // Perform the SPI transaction
    esp_err_t check_spi_write_8 = spi_device_polling_transmit(spi_handle, &trans);

    // Check for errors and stop the program if the transaction fails
    ESP_ERROR_CHECK(check_spi_write_8);
}

void spi_write_16(uint32_t address, uint16_t data)
{
    // FT812 requires: [WRITE_CMD(0x41)] + [24-bit address] + [16-bit data]

    uint8_t tx_buf[6];

    tx_buf[0] = WRITE;                  // WRITE command
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = (address) & 0xFF;       // Address byte 0 (LSB)
    tx_buf[4] = data;                   // Data byte 1 to write
    tx_buf[5] = (data >> 8) & 0xFF;     // Data to byte 2 to write

    spi_transaction_t trans = {
        .length = 6 * 8, // Total bits to send (5 bytes)
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};

    // Perform the SPI transaction
    esp_err_t check_spi_write_16 = spi_device_polling_transmit(spi_handle, &trans);

    // Check for errors and stop the program if the transaction fails
    ESP_ERROR_CHECK(check_spi_write_16);
}

void spi_write_32(uint32_t address, uint32_t data)
{
    // FT812 requires: [WRITE_CMD(0x41)] + [24-bit address] + [32-bit data]

    uint8_t tx_buf[8];

    tx_buf[0] = WRITE;                  // WRITE command
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = (address) & 0xFF;       // Address byte 0 (LSB)
    tx_buf[4] = data;                   // Data byte 1 to write
    tx_buf[5] = (data >> 8) & 0xFF;     // Data to byte 2 to write
    tx_buf[6] = (data >> 16) & 0xFF;    // Data to byte 3 to write
    tx_buf[7] = (data >> 24) & 0xFF;    // Data to byte 4 to write

    spi_transaction_t trans = {
        .length = 8 * 8, // Total bits to send (8 bytes)
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};

    // Perform the SPI transaction
    esp_err_t check_spi_write_32 = spi_device_polling_transmit(spi_handle, &trans);

    // Check for errors and stop the program if the transaction fails
    ESP_ERROR_CHECK(check_spi_write_32);
}

uint8_t spi_read_8(uint32_t address)
{
    // FT812 requires: [READ_CMD(0x80)] + [24-bit address] + [dummy byte]

    uint8_t tx_buf[4]; // Command + 24-bit address
    uint8_t rx_buf[4]; // Response buffer (1 dummy byte + 1 data byte)

    // Prepare the transmit buffer
    tx_buf[0] = READ;                   // READ command
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = (address) & 0xFF;       // Address byte 0 (LSB)

    // Configure the SPI transaction
    spi_transaction_t trans = {
        .length = 4 * 8,     // Total bits to send (4 bytes)
        .rxlength = 4 * 8,   // Total bits to receive (4 bytes)
        .tx_buffer = tx_buf, // Pointer to the transmit buffer
        .rx_buffer = rx_buf  // Pointer to the receive buffer
    };

    // Perform the SPI transaction
    esp_err_t check_spi_read_8 = spi_device_polling_transmit(spi_handle, &trans);

    // Check for errors and stop the program if the transaction fails
    ESP_ERROR_CHECK(check_spi_read_8);

    // Return the received data byte (skip the dummy byte)
    return rx_buf[3];
}

uint16_t spi_read_16(uint32_t address)
{
    // FT812 requires: [READ_CMD(0x80)] + [24-bit address] + [dummy byte]

    uint8_t tx_buf[4]; // Command + 24-bit address
    uint8_t rx_buf[6]; // Response buffer (1 dummy byte + 2 data byte)

    // Prepare the transmit buffer
    tx_buf[0] = READ;                   // READ command
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = (address) & 0xFF;       // Address byte 0 (LSB)

    // Configure the SPI transaction
    spi_transaction_t trans = {
        .length = 4 * 8,     // Total bits to send (4 bytes)
        .rxlength = 6 * 8,   // Total bits to receive (6 bytes)
        .tx_buffer = tx_buf, // Pointer to the transmit buffer
        .rx_buffer = rx_buf  // Pointer to the receive buffer
    };

    // Perform the SPI transaction
    esp_err_t check_spi_read_16 = spi_device_polling_transmit(spi_handle, &trans);

    // Check for errors and stop the program if the transaction fails
    ESP_ERROR_CHECK(check_spi_read_16);

    // Combine the two data bytes into a 16-bit value (skip the dummy byte)
    uint16_t data = (rx_buf[4] | (rx_buf[5] << 8));
    return data;
}

uint32_t spi_read_32(uint32_t address)
{
    // FT812 requires: [READ_CMD(0x80)] + [24-bit address] + [dummy byte]

    uint8_t tx_buf[4]; // Command + 24-bit address
    uint8_t rx_buf[8]; // Response buffer (1 dummy byte + 2 data byte)

    // Prepare the transmit buffer
    tx_buf[0] = READ;                   // READ command
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = (address) & 0xFF;       // Address byte 0 (LSB)

    // Configure the SPI transaction
    spi_transaction_t trans = {
        .length = 4 * 8,     // Total bits to send (4 bytes)
        .rxlength = 8 * 8,   // Total bits to receive (6 bytes)
        .tx_buffer = tx_buf, // Pointer to the transmit buffer
        .rx_buffer = rx_buf  // Pointer to the receive buffer
    };

    // Perform the SPI transaction
    esp_err_t check_spi_read_32 = spi_device_polling_transmit(spi_handle, &trans);

    // Check for errors and stop the program if the transaction fails
    ESP_ERROR_CHECK(check_spi_read_32);

    // Combine the two data bytes into a 16-bit value (skip the dummy byte)
    uint16_t data = (rx_buf[4] | (rx_buf[5] << 8) | (rx_buf[6] << 16) | (rx_buf[7] << 24));
    return data;
}

void host_command(uint8_t command) {
    spi_transaction_t trans = {
        .length = 8,            // Single byte command
        .tx_buffer = &command,      // Pointer to command byte
        .rx_buffer = NULL       // No response expected
    };

    // Execute the transaction
    esp_err_t check_host_command = spi_device_polling_transmit(spi_handle, &trans);
        ESP_LOGE(SPI_TAG, "Host command 0x%02X failed: %s", command, esp_err_to_name(check_host_command));
    
    // Some commands require a delay
    switch(command) {
        case 0x68: // CORERST
        case 0x57: // CLK48M
            vTaskDelay(pdMS_TO_TICKS(20)); // 20ms delay
            break;
    }
}