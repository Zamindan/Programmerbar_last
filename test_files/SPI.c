#include <stdio.h>
#include "SPI.h"

#include "hal/spi_types.h"
#include "hal/spi_common.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"


#define SPI_SCLK 12 
#define SPI_CS 10 

// FOR Standard SPI, the following pins are used:
#define SPI_MISO 11
#define SPI_MOSI 13

// FOR QUAD SPI, the following pins need to be used:
/*
#define SPI_data_0 13 
#define SPI_data_1 11
#define SPI_data_2 14
#define SPI_data_3 9
*/


// SPI2 will be used for communication with LCD
spi_host_device_t SPI_type = SPI2_HOST;

// Driver chooses automaticely which DMA channel to use
spi_dma_chan_t = SPI_DMA_CH_AUTO;


// Configurated SPI bus for QUAD SPI 2
spi_bus_config_t bus_config = {
    .sclk_io_num = SPI_SCLK,
    .mosi_io_num = SPI_MOSI,
    .miso_io_num = SPI_MISO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .data0_io_num = -1,
    .data1_io_num = -1,
    .data2_io_num = -1,
    .data3_io_num = -1,
    .data4_io_num = -1,
    .data5_io_num = -1,
    .data6_io_num = -1,
    .data7_io_num = -1,
    .max_transfer_sz = 4092, 
}

const spi_device_interface_config_t device_configuration = {
    .clock_speed_hz = SPI_MASTER_FREQ_20M; // 20MHz clock
    .spics_io_num = SPI_CS;                // Enable CS pin
    .mode 0;                               // CPOL = 0 & CPHA = 0
}

spi_transaction_t SPI_transaction = {
    .
}

void spi_bus_add_device(SPI_type, *device_configuration, spi_device_handle_t *handle)

void spi_bus_remove_device(spi_device_handle_t *handle)

esp_err_t spi_device_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc)

void app_main()
{}
