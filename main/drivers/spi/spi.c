#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include <inttypes.h>



#define SPI_TAG "SPI"

spi_device_handle_t spi_handle;
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
    // FT812 requires: [WRITE_CMD(0x41)] + [24-bit address] + [8-bit data]
    uint8_t tx_buf[5]; // 1 (cmd) + 3 (addr) + 1 (data)

    tx_buf[0] = 0x41;                   // WRITE command (0x41)
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = address & 0xFF;         // Address byte 0 (LSB)
    tx_buf[4] = data;                   // 8-bit data

    spi_transaction_t trans = {
        .length = 5 * 8, // Total bits to send (5 bytes)
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};

    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    printf("SPI  8 bit Write, Address: 0x%02X%02X%02X, Data: 0x%02X\n", tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[4]);
}

void spi_write_16(uint32_t address, uint16_t data)
{
    // FT812 requires: [WRITE_CMD(0x41)] + [24-bit address] + [16-bit data]
    uint8_t tx_buf[6]; // 1 (cmd) + 3 (addr) + 2 (data)

    tx_buf[0] = 0x41;                   // WRITE command (0x41)
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = address & 0xFF;         // Address byte 0 (LSB)
    tx_buf[4] = data & 0xFF;            // Data LSB
    tx_buf[5] = (data >> 8) & 0xFF;     // Data MSB

    spi_transaction_t trans = {
        .length = 6 * 8, // Total bits to send (6 bytes)
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};

    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    printf("SPI 16 bit Write, Address: 0x%02X%02X%02X, Data: 0x%02X%02X\n", tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[5], tx_buf[4]);
}

void spi_write_32(uint32_t address, uint32_t data)
{
    // FT812 requires: [WRITE_CMD(0x41)] + [24-bit address] + [32-bit data]
    uint8_t tx_buf[8];

    tx_buf[0] = 0x41;                   // WRITE command (0x41)
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = address & 0xFF;         // Address byte 0 (LSB)
    tx_buf[4] = data & 0xFF;            // Data byte 0 (LSB)
    tx_buf[5] = (data >> 8) & 0xFF;     // Data byte 1
    tx_buf[6] = (data >> 16) & 0xFF;    // Data byte 2
    tx_buf[7] = (data >> 24) & 0xFF;    // Data byte 3 (MSB)

    spi_transaction_t trans = {
        .length = 8 * 8, // Total bits to send (8 bytes)
        .tx_buffer = tx_buf,
        .rx_buffer = NULL};

    // Perform the SPI transaction
    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    printf("SPI 32 bit Write, Address: 0x%02X%02X%02X, Data: 0x%02X%02X%02X%02X\n", tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[7], tx_buf[6], tx_buf[5], tx_buf[4]);
}

uint8_t spi_read_8(uint32_t address)
{
    // First SPI transfer: send command (read + address + dummy)
    uint8_t tx_buf[5] = {
        0x00,                             // Correct FT812 read command
        (address >> 16) & 0xFF,
        (address >> 8) & 0xFF,
        address & 0xFF,
        0x00                              // Dummy byte
    };
    uint8_t rx_buf[5] = {0};

    spi_transaction_t trans = {
        .length = 5 * 8,
        .rxlength = 5 * 8,
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf
    };

    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    // Second SPI transfer: clock out the actual data
    uint8_t dummy_tx = 0x00;
    uint8_t data_buf = 0x00;

    spi_transaction_t data_trans = {
        .length = 8,
        .rxlength = 8,
        .tx_buffer = &dummy_tx,
        .rx_buffer = &data_buf
    };

    ret = spi_device_polling_transmit(spi_handle, &data_trans);
    ESP_ERROR_CHECK(ret);

    printf("SPI  8-bit Read, Addr: 0x%06lX, Data: 0x%02X\n", address, data_buf);
    return data_buf;
}


uint16_t spi_read_16(uint32_t address)
{
    uint8_t tx_buf[5] = {
        0x00,                             // Correct read command
        (address >> 16) & 0xFF,           // Address MSB
        (address >> 8) & 0xFF,            // Address mid
        address & 0xFF,                   // Address LSB
        0x00                              // Dummy byte (ignored)
    };

    uint8_t rx_buf[5] = {0};

    spi_transaction_t trans = {
        .length = 5 * 8,       // 5 bytes out
        .rxlength = 5 * 8,     // Receive 5 bytes (last 2 are the data)
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf
    };

    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    // Received bytes: rx_buf[0-3] are garbage (including dummy), rx_buf[4] is first valid byte
    // But due to full-duplex nature, data may be at [4] and [5], depending on timing
    // So safest way is to send the 4 command bytes first, then do a separate read:

    uint8_t data_buf[2] = {0x00, 0x00};
    tx_buf[0] = 0x00; // Send dummy bytes to clock out data

    spi_transaction_t data_trans = {
        .length = 2 * 8,
        .tx_buffer = tx_buf, // Just use same buffer with dummy bytes
        .rx_buffer = data_buf
    };

    ret = spi_device_polling_transmit(spi_handle, &data_trans);
    ESP_ERROR_CHECK(ret);

    uint16_t data = (data_buf[0] << 8) | data_buf[1];

    printf("SPI 16-bit Read, Addr: 0x%06lX, Data: 0x%04X\n", address, data);
    return data;
}


uint32_t spi_read_32(uint32_t address)
{
    // FT812 requires: [READ_CMD(0x80)] + [24-bit address] + [dummy byte]

    uint8_t tx_buf[5]; // Command + 24-bit address + dummy byte
    uint8_t rx_buf[8]; // Dummy byte + 4 data bytes

    // Prepare the transmit buffer
    tx_buf[0] = 0x80;                   // READ command (0x80)
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = address & 0xFF;         // Address byte 0 (LSB)
    tx_buf[4] = 0x00;                   // Dummy byte needed for read operation

    // Configure the SPI transaction
    spi_transaction_t trans = {
        .length = 5 * 8,     // Total bits to send (5 bytes)
        .rxlength = 5 * 8,   // Total bits to receive (5 bytes)
        .tx_buffer = tx_buf, // Pointer to the transmit buffer
        .rx_buffer = rx_buf  // Pointer to the receive buffer
    };

    // Perform the SPI transaction
    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    // Combine the received bytes (skip first dummy byte)
    uint32_t data = (rx_buf[4] << 24) | (rx_buf[3] << 16) | (rx_buf[2] << 8) | rx_buf[1];

    printf("SPI 32 bit Read, Address: 0x%02X%02X%02X, Data: 0x%02X%02X%02X%02X\n", tx_buf[1], tx_buf[2], tx_buf[3], rx_buf[4], rx_buf[3], rx_buf[2], rx_buf[1]);
    return data;
}

// Function to send a host command
void host_command(uint8_t command) {
    
    uint8_t tx_data[3] = { command, 0x00, 0x00 };

    spi_transaction_t t = {
        .length = 3 * 8, // in bits
        .tx_buffer = tx_data,
        .rx_buffer = NULL,
    };

    esp_err_t ret = spi_device_transmit(spi_handle, &t);
    if (ret != ESP_OK) {
        printf("SPI transmission failed: %s\n", esp_err_to_name(ret));
    } else {
        printf("Host command 0x%02X sent.\n", command);
    }
}