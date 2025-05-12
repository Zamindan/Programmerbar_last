#ifndef SPI_H

/**
 * @file spi.h
 * @brief Header file for SPI functions.
 *
 * This file contains the declarations for SPI initialization, device addition,
 * read and write transmission.
 *
 * @date 2025-04-15
 */

/**
 * @brief Function for initializing SPI communication protocol
 *
 * @param SCK Select pin for SPI clock
 * @param MOSI Select pin for MOSI (Master Out Slave In)
 * @param MISO Select pin for MISO (Master In Slave Out)
 * @param SPI_HOST Select type of SPI (SPI1, SPI2 or SPI3)
 */
void spi_init(gpio_num_t SCK, gpio_num_t MOSI, gpio_num_t MISO, spi_host_device_t SPI_HOST);

/**
 * @brief Function for adding a device to the SPI bus
 *
 * @param clk_speed Clock speed for SPI bus
 * @param duty_val Duty cycle value for the SPI bus
 * @param spi_mode SPI mode (0: CPOL=0, CPHA=0, 1: CPOL=0, CPHA=1, 2: CPOL=1, CPHA=0, 3: CPOL=1, CPHA=1)
 * @param spi_queue_size Size of the SPI queue
 * @param CS Select pin for chip select
 * @param SPI_HOST Select type of SPI (SPI1, SPI2 or SPI3)
 */
void spi_add_device(int clk_speed, int duty_val, int spi_mode, int spi_queue_size, gpio_num_t CS, spi_host_device_t SPI_HOST);

/**
 * @brief Function for writing 8-bit data to a specific address using standard SPI
 *
 * @param address Register address to write to
 * @param data 8 bit data to write to the register
 */
void spi_write_8(uint32_t address, uint8_t data);

/**
 * @brief Function for writing 8-bit string to a specific address using standard SPI
 * 
 * @param address Register address to write to
 */
// void spi_write_8_string(uint32_t address);

/**
 * @brief Function for writing 16-bit data to a specific address using standard SPI
 *
 * @param address Register address to write to
 * @param data 16 bit data to write to the register
 */

void spi_write_16(uint32_t address, uint16_t data);

/**
 * @brief Function for writing 32-bit data to a specific address using standard SPI
 *
 * @param address Register address to write to
 * @param data 32 bit data to write to the register
 */
void spi_write_32(uint32_t address, uint32_t data);

/**
 * @brief Function for reading 8-bit data from a specific address using standard SPI
 *
 * @param address Register address to read from
 * @return uint8_t 8 bit data read from the register
 */
uint8_t spi_read_8(uint32_t address);

/**
 * @brief Function for reading 8-bit string from a specific address using standard SPI
 *
 * @param address Register address to read from
 */
// void spi_read_8_string(uint32_t address);

/**
 * @brief Function for reading 16-bit data from a specific address using standard SPI
 *
 * @param address Register address to read from
 * @return uint16_t 16 bit data read from the register
 */
uint16_t spi_read_16(uint32_t address);

/**
 * @brief Function for reading 32-bit data from a specific address using standard SPI
 *
 * @param address Register address to read from
 * @return uint32_t 32 bit data read from the register
 */
uint32_t spi_read_32(uint32_t address);

#endif