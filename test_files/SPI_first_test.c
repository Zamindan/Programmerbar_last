#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"


// Defining pins needed for SPI
#define SPI_SCLK 12 
#define SPI_CS 10 
#define SPI_MISO 13
#define SPI_MOSI 11

#define SPI_HOST SPI2_HOST
#define DMA_CH SPI_DMA_CH_AUTO

void app_main(void)
{
    spi_device_handle_t handle;

    spi_bus_config_t bus_config = {
        .sclk_io_num = SPI_SCLK,
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
     
        spi_device_interface_config_t device_configuration = {
            .clock_speed_hz = 1000000,             // Clock speed of 10MHzid
            .duty_cycle_pos = 128,                 // Duty cycle of 128
            .spics_io_num = SPI_CS,                // Enable CS pin
            .mode = 0,                               // CPOL = 0 & CPHA = 0
            .queue_size = 1,
        };

    spi_bus_initialize(SPI_HOST, &bus_config, DMA_CH);
    spi_bus_add_device(SPI_HOST, &device_configuration, &handle);

    char TX_buf[128] = {0};
    char RX_buf[128] = {0};

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    while(1)
    {
        snprintf(TX_buf, sizeof(TX_buf), "Testing Fast SPI for ESP32");
        t.length = sizeof(TX_buf) * 8;
        t.tx_buffer = TX_buf;
        t.rx_buffer = RX_buf;
        spi_device_transmit(handle, &t);
        printf("Message sendt from MOSI: %s\n", TX_buf);
        printf("Message received from MISO: %s\n", RX_buf);
        vTaskDelay(pdMS_TO_TICKS(1000));
        
    };

    
} 
