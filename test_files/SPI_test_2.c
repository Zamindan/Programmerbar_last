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
#define GPIO_CLK GPIO_NUM_12
#define GPIO_CS GPIO_NUM_10
#define GPIO_PD GPIO_NUM_18 // Power Down (PD#) Pin

#define DMA_CH SPI_DMA_CH_AUTO

void spi_init(spi_host_device_t SPI_HOST_id, gpio_num_t SPI_SCLK, gpio_num_t SPI_MOSI, gpio_num_t SPI_MISO, spi_dma_chan_t DMA_CH)
{
    spi_bus_config_t bus_config = {
        .sclk_io_num = SPI_SCLK,
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .flags = 0,
    };

    esp_err_t check_SPI_init = spi_bus_initialize(SPI_HOST_id, &bus_config, DMA_CH);
    ESP_LOGE(SPI_TAG, "SPI Init: %s", esp_err_to_name(check_SPI_init));
    vTaskDelay(pdMS_TO_TICKS(10));
}

void spi_add_device(spi_host_device_t SPI_HOST_id, int clk_speed, int duty_val, int SPI_queue_size, gpio_num_t SPI_CS, spi_device_handle_t *handle_name)
{
    gpio_set_direction(SPI_CS, GPIO_MODE_OUTPUT);

    spi_device_interface_config_t device_configuration = {
        .clock_speed_hz = clk_speed,
        .duty_cycle_pos = duty_val,
        .mode = 0,
        .queue_size = SPI_queue_size,
        .spics_io_num = SPI_CS,
        .flags = 0,
    };

    esp_err_t check_spi_add_device = spi_bus_add_device(SPI_HOST_id, &device_configuration, handle_name);
    ESP_LOGE(SPI_TAG, "SPI add device: %s", esp_err_to_name(check_spi_add_device));

    vTaskDelay(pdMS_TO_TICKS(10));
}

uint32_t spi_read(uint32_t reg_address, spi_device_handle_t *handle_name)
{
    uint8_t tx_buffer[3] = {
        (reg_address >> 16) & 0xFF,
        (reg_address >> 8) & 0xFF,
        reg_address & 0xFF
    };

    uint8_t *rx_buffer = heap_caps_malloc(4, MALLOC_CAP_DMA);
    if (!rx_buffer)
    {
        ESP_LOGE("SPI", "Failed to allocate DMA memory for RX buffer");
        return 0;
    }

    spi_transaction_t trans = {
        .flags = 0,
        .tx_buffer = tx_buffer,
        .rx_buffer = rx_buffer,
        .length = 3 * 8,
        .rxlength = 4 * 8
    };

    gpio_set_level(GPIO_CS, 0);
    esp_err_t transaction_check = spi_device_polling_transmit(*handle_name, &trans);
    gpio_set_level(GPIO_CS, 1);

    if (transaction_check != ESP_OK)
    {
        ESP_LOGE("SPI", "SPI read failed");
        free(rx_buffer);
        return 0;
    }

    uint32_t value = (rx_buffer[3] << 24) | (rx_buffer[2] << 16) | (rx_buffer[1] << 8) | rx_buffer[0];
    free(rx_buffer);
    return value;
}

void spi_write(uint32_t reg_address, uint32_t data, spi_device_handle_t *handle_name)
{
    uint8_t *tx_buffer = heap_caps_malloc(7, MALLOC_CAP_DMA);
    if (!tx_buffer)
    {
        ESP_LOGE("SPI", "Failed to allocate DMA memory for TX buffer");
        return;
    }

    tx_buffer[0] = (reg_address >> 16) & 0xFF;
    tx_buffer[1] = (reg_address >> 8) & 0xFF;
    tx_buffer[2] = reg_address & 0xFF;

    tx_buffer[3] = data & 0xFF;
    tx_buffer[4] = (data >> 8) & 0xFF;
    tx_buffer[5] = (data >> 16) & 0xFF;
    tx_buffer[6] = (data >> 24) & 0xFF;

    spi_transaction_t trans = {
        .flags = 0,
        .tx_buffer = tx_buffer,
        .length = 7 * 8,
    };

    gpio_set_level(GPIO_CS, 0);
    esp_err_t transaction_check = spi_device_polling_transmit(*handle_name, &trans);
    gpio_set_level(GPIO_CS, 1);

    if (transaction_check != ESP_OK)
    {
        ESP_LOGE(SPI_TAG, "SPI write operation failed");
    }

    free(tx_buffer);
}

void app_main()
{
    spi_device_handle_t SPI_handler;
    spi_init(SPI_HOST, GPIO_CLK, GPIO_MOSI, GPIO_MISO, DMA_CH);
    spi_add_device(SPI_HOST, 1000000, 0, 1, GPIO_CS, &SPI_handler);

    uint32_t addr = 0x00000022;
    uint32_t data = 0x000000AA;

    spi_read(addr, &SPI_handler);
    spi_write(addr, data, &SPI_handler);
}
