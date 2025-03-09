#ifndef SPI_H
#define SPI_H

/**
 * @file SPI_test.c
 * @brief Implementation of SPI initialization and reading functions.
 * 
 * This file contains necesearly function for SPI communication. 
 * It includes the following functions: SPI initialization, device addition, 
 * writing and reading data
 * 
 * @author Ardit Uka
 * @date 2025-02-22
 */


/**
 * @brief Function for initializing SPI
 *  
 * @param SPI_host_id   Select which SPI to use
 * @param SPI_SCLK      Select SPI clock GPIO
 * @param SPI_MOSI      Select MOSI GPIO
 * @param SPI_MISO      Select MISO GPIO
 * @param DMA_CH        Select a DMA channel for an SPI bus 
 * 
 * 
 */
void SPI_init(spi_host_device_t SPI_HOST_id ,gpio_num_t SPI_SCLK, gpio_num_t SPI_MOSI, gpio_num_t SPI_MISO, spi_dma_chan_t DMA_CH)

/**
 * @brief Function for adding SPI device
 * 
 * @param SPI_HOST_id Select which SPI to use
 * @param clk_speed Set clock speed for SPI
 * @param duty_val Scale down the clock to match the communication speed
 * @param SPI_queue_size Choosing how many operation can occure at once
 * @param SPI_CS Select CS GPIO
 * @param hande_name Choose a handler for SPI operation
 * 
 */
void SPI_add_device(spi_host_device_t SPI_HOST_id, int clk_speed, int duty_val,, int SPI_queue_size, gpio_num_t SPI_CS, spi_device_handle_t *handle_name)
