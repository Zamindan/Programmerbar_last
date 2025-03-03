#ifndef SPI_H
#define SPI_H

/**
 * @file SPI_test.c
 * @brief Implementation of SPI initialization and reading functions.
 * 
 * This file contains the declarations for SPI initialization, device addition, 
 * and data transmission functions.
 * 
 * @author Ardit Uka
 * @date 2025-02-22
 */

#include "hal/spi_types.h"
#include "hal/spi_common.h"
#include "driver/spi_master.h"

/**
 * @brief Initialize SPI bus
 * @param host_id SPI peripheral that controls the bus
 * @param bus_config specifying how the host should be initialized
 * @param dma_chan selecting a DMA channel
 */
void spi_bus_initialize(spi_host_device_t host_id, const spi_bus_config_t *bus_config, spi_dma_chan_t dma_chan)

/**
 * @brief Free a SPI bus
 * @param host_id SPI peripheral to free
 */
void spi_bus_free(spi_host_device_t host_id)

/**
 * @brief 
 * @param host_id SPI peripheral who will using the memeory
 * @param size amoun of memory to allocate in bytes
 * @param extra_heap_caps Extra heap caps based on MALLOC_CAP_DMA
 */
void *spi_bus_dma_memory_alloc(spi_host_device_t host_id, size_t size, uint32_t extra_heap_caps)


////////////////////////////MASTER API//////////////////////////////////////

/**
 * @brief Add a device on the SPI bus
 * @param host_id preiferal to add
 * @param dev_config protocol configuration
 * @param handle pointer to variable to hold the device handle
 */

spi_bus_add_device(spi_host_device_t host_id, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle)

/**
 * @brief Remove a device on the SPI bus
 * @param ?????
 */
esp_err_t spi_bus_remove_device(spi_device_handle_t handle)

 #endif

 esp_err_t spi_device_get_trans_result(spi_device_handle_t handle, spi_transaction_t **trans_desc, TickType_t ticks_to_wait)

 esp_err_t spi_device_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc)

 esp_err_t spi_device_polling_start(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait)

 esp_err_t spi_device_acquire_bus(spi_device_handle_t device, TickType_t wait)

 void spi_device_release_bus(spi_device_handle_t dev)

 esp_err_t spi_device_get_actual_freq(spi_device_handle_t handle, int *freq_khz)

 void spi_get_timing(bool gpio_is_used, int input_delay_ns, int eff_clk, int *dummy_o, int *cycles_remain_o)

 esp_err_t spi_bus_get_max_transaction_len(spi_host_device_t host_id, size_t *max_bytes)