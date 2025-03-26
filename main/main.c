#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Pin definitions
#define PIN_NUM_MISO  -1  // Not used for this display
#define PIN_NUM_MOSI  13
#define PIN_NUM_CLK   14
#define PIN_NUM_CS    10
#define PIN_NUM_DC    12
#define PIN_NUM_RST   11
#define PIN_NUM_BKL   9   // Backlight control

// SPI configuration
spi_device_handle_t spi;

void initialize_spi() {
    esp_err_t ret;
    
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };
    
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,  // 10MHz
        .mode = 0,                            // SPI mode 0
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7,
        .pre_cb = NULL,
        .post_cb = NULL,
    };
    
    // Initialize the SPI bus
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    
    // Attach the LCD to the SPI bus
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
}

void initialize_gpio() {
    // Configure reset and data/command pins as outputs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST) | (1ULL << PIN_NUM_BKL),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    // Set initial states
    gpio_set_level(PIN_NUM_RST, 1);
    gpio_set_level(PIN_NUM_DC, 0);
    gpio_set_level(PIN_NUM_BKL, 1);  // Turn on backlight
}

void lcd_reset() {
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void lcd_send_cmd(uint8_t cmd) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    
    gpio_set_level(PIN_NUM_DC, 0);  // Command mode
    t.length = 8;
    t.tx_buffer = &cmd;
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

void lcd_send_data(uint8_t *data, uint16_t length) {
    esp_err_t ret;
    spi_transaction_t t;
    
    if (length == 0) return;
    
    memset(&t, 0, sizeof(t));
    gpio_set_level(PIN_NUM_DC, 1);  // Data mode
    t.length = length * 8;
    t.tx_buffer = data;
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

void lcd_init() {
    // Initialize GPIO and SPI
    initialize_gpio();
    initialize_spi();
    
    // Reset the display
    lcd_reset();
    
    // Send initialization commands
    // Refer to your display's datasheet for the exact command sequence
    lcd_send_cmd(0x01);  // Example command - replace with actual commands
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    // More initialization commands...
}

void app_main() {
    lcd_init();
    
    // Now you can start drawing on the display
    // You'll need to implement functions to set drawing area, write pixels, etc.
}