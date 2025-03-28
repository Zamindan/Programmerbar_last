#include "driver/spi_master.h"
#include "esp_log.h"

static const char *TAG = "SPI_DRIVER";

// SPI device handle
static spi_device_handle_t spi_handle;

/**
 * @brief Initialize the SPI bus and attach a device.
 *
 * @param mosi_pin GPIO number for MOSI.
 * @param miso_pin GPIO number for MISO.
 * @param sclk_pin GPIO number for SCLK.
 * @param cs_pin GPIO number for Chip Select (CS).
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
void spi_init(int mosi_pin, int miso_pin, int sclk_pin, int cs_pin) {

    // SPI bus configuration
    spi_bus_config_t buscfg = {
        .mosi_io_num = mosi_pin,
        .miso_io_num = miso_pin,
        .sclk_io_num = sclk_pin,
        .quadwp_io_num = -1, // Not used
        .quadhd_io_num = -1, // Not used
        .max_transfer_sz = 4096 // Maximum transfer size in bytes
    };

    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // SPI device configuration
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000, // 1 MHz
        .mode = 0,                         // SPI mode 0 (CPOL=0, CPHA=0)
        .spics_io_num = cs_pin,            // CS pin
        .queue_size = 7,                   // Transaction queue size
        .flags = 0,                        // No special flags
    };

    // Attach the device to the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle));
    ESP_LOGI(TAG, "SPI initialized successfully");
}

/**
 * @brief Write data to the SPI device.
 *
 * @param data Pointer to the data to send.
 * @param length Length of the data in bytes.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
void spi_write(const uint8_t *data, size_t length) {
    spi_transaction_t transaction = {
        .length = length * 8, // Length in bits
        .tx_buffer = data,    // Pointer to the data to send
        .rx_buffer = NULL     // No receive buffer
    };

    ESP_ERROR_CHECK(spi_device_transmit(spi_handle, &transaction));
}

/**
 * @brief Read data from the SPI device.
 *
 * @param data Pointer to the buffer to store received data.
 * @param length Length of the data to read in bytes.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
void spi_read(uint8_t *data, size_t length) {
    spi_transaction_t transaction = {
        .length = length * 8, // Length in bits
        .tx_buffer = NULL,    // No data to send
        .rx_buffer = data     // Pointer to the buffer to store received data
    };

    ESP_ERROR_CHECK(spi_device_transmit(spi_handle, &transaction));
}