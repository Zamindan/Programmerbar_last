#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SPI_TAG "SPI"
#define LCD_TAG "LCD"

#define SPI_HOST SPI2_HOST // Use SPI2 (HSPI)
#define GPIO_MISO GPIO_NUM_13
#define GPIO_MOSI GPIO_NUM_11
#define GPIO_SCLK GPIO_NUM_12
#define GPIO_CS GPIO_NUM_10
#define GPIO_PD GPIO_NUM_8 // Power Down (PD#) Pin

// Memory map (Start addresses)
#define RAM_G 0x000000
#define ROM_FONT 0x1E0000
#define ROM_FONT_ADDR 0x2FFFFC
#define RAM_DL 0x300000
#define RAM_REG 0x302000
#define RAM_CMD 0x308000

// Register Address Definitions
#define REG_TAP_MASK 0x302028
#define REG_HCYCLE 0x30202C
#define REG_HOFFSET 0x302030
#define REG_HSIZE 0x302034
#define REG_HSYNC0 0x302038
#define REG_HSYNC1 0x30203C
#define REG_VCYCLE 0x302040
#define REG_VOFFSET 0x302044
#define REG_VSIZE 0x302048
#define REG_VSYNC0 0x30204C
#define REG_VSYNC1 0x302050
#define REG_DLSWAP 0x30205A
#define REG_ROTATE 0x302058
#define REG_OUTBITS 0x30205C
#define REG_DITHER 0x302060
#define REG_SWIZZLE 0x30206A
#define REG_CSPREAD 0x302068
#define REG_PCLK_POL 0x30206C
#define REG_PCLK 0x302070
#define REG_TAG_X 0x30207A
#define REG_TAG_Y 0x302078
#define REG_TAG 0x30207C
#define REG_VOL_PB 0x302080
#define REG_VOL_SOUND 0x30208A
#define REG_SOUND 0x302088
#define REG_PLAY 0x30208C
#define REG_GPIO_DIR 0x302090
#define REG_GPIO 0x302094
#define REG_GPIOX_DIR 0x302098
#define REG_GPIOX 0x30209C
#define REG_INT_FLAGS 0x3020A8
#define REG_INT_EN 0x3020AC
#define REG_INT_MASK 0x3020B0
#define REG_PLAYBACK_START 0x3020B4
#define REG_PLAYBACK_LENGTH 0x3020B8
#define REG_PLAYBACK_READPTR 0x3020BC
#define REG_PLAYBACK_FREQ 0x3020C0
#define REG_PLAYBACK_FORMAT 0x3020C4
#define REG_PLAYBACK_LOOP 0x3020C8
#define REG_PLAYBACK_PLAY 0x3020CC
#define REG_PWM_HZ 0x3020D0
#define REG_PWM_DUTY 0x3020D4
#define REG_MACRO_0 0x3020D8
#define REG_MACRO_1 0x3020DC
#define REG_CMD_READ 0x3020F8
#define REG_TOUCH_TAG3_XY 0x302140
#define REG_TOUCH_TAG3 0x302144
#define REG_TOUCH_TAG4_XY 0x302148
#define REG_TOUCH_TAG4 0x30214C
#define REG_TOUCH_TRANSFORM_A 0x302150
#define REG_TOUCH_TRANSFORM_B 0x302154
#define REG_TOUCH_TRANSFORM_C 0x302158
#define REG_TOUCH_TRANSFORM_D 0x30215C
#define REG_TOUCH_TRANSFORM_E 0x302160
#define REG_TOUCH_TRANSFORM_F 0x302164
#define REG_TOUCH_CONFIG 0x302168
#define REG_CTOUCH_TOUCH4_X 0x30216C
#define REG_BIST_EN 0x302174
#define REG_TRIM 0x302180
#define REG_ANA_COMP 0x30218A
#define REG_SPI_WIDTH 0x30218B
#define REG_TOUCH_DIRECT_XY 0x30218C
#define REG_CTOUCH_TOUCH2_XY 0x30218C
#define REG_TOUCH_DIRECT_Z12 0x302190
#define REG_CTOUCH_TOUCH3_XY 0x302190
#define REG_DATESTAMP 0x30256A
#define REG_CMDB_SPACE 0x30257A
#define REG_CMDB_WRITE 0x30257B

#define CLOCK_10_MHz 10000000
#define CLOCK_30_MHz 30000000

// Host Command
#define ACTIVE 0x00
#define STANDBY 0x41
#define SLEEP 0x42
#define PWRDOWN 0x43
#define CLKEXT 0x44
#define CLKINT 0x48
#define PD_ROMS 0x49
#define RST_PULSE 0x68
#define PINDRIVE 0x70
#define PIN_PD_STATE 0x71

// Read/Write Commands
#define WRITE 0x41
#define READ 0x80

// Drawing Primitives
#define BITMAPS 1
#define POINTS 2
#define LINES 3        // Line drawing primitive
#define LINE_STRIP 4   // Line strip drawing primitive
#define EDGE_STRIP_R 5 // Edge strip right side drawing primitive
#define EDGE_STRIP_L 6 // Edge strip left side drawing primitive
#define EDGE_STRIP_A 7 // Edge strip above drawing primitive
#define EDGE_STRIP_B 8 // Edge strip below side drawing primitive
#define RECTS 9        // Rectangle drawing primitive

// SPI INIT file:

spi_device_handle_t spi_handle;

void spi_init(gpio_num_t SCLK, gpio_num_t MOSI, gpio_num_t MISO)
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
}

void spi_write_32(uint32_t address, uint32_t data)
{
    // FT812 requires: [WRITE_CMD(0x41)] + [24-bit address] + [32-bit data]
    uint8_t tx_buf[8];

    tx_buf[0] = WRITE;                  // WRITE command (0x41)
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
}

uint8_t spi_read_8(uint32_t address)
{
    // FT812 requires: [READ_CMD(0x80)] + [24-bit address] + [dummy byte]
    uint8_t tx_buf[5]; // Command + 24-bit address + dummy byte
    uint8_t rx_buf[3]; // Dummy byte + 1 data byte

    // Prepare the transmit buffer
    tx_buf[0] = 0x80;                   // READ command (0x80)
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = address & 0xFF;         // Address byte 0 (LSB)
    tx_buf[4] = 0x00;                   // Dummy byte needed for read operation

    // Configure the SPI transaction
    spi_transaction_t trans = {
        .length = 5 * 8,     // Total bits to send (5 bytes)
        .rxlength = 2 * 8,   // Total bits to receive (2 bytes: dummy + 1 data)
        .tx_buffer = tx_buf, // Pointer to the transmit buffer
        .rx_buffer = rx_buf  // Pointer to the receive buffer
    };

    // Perform the SPI transaction
    esp_err_t ret = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(ret);

    // Return the received data byte (skip first dummy byte)
    return rx_buf[1];
}

uint16_t spi_read_16(uint32_t address)
{
    // FT812 requires: [READ_CMD(0x80)] + [24-bit address] + [dummy byte]

    uint8_t tx_buf[5]; // Command + 24-bit address + dummy byte
    uint8_t rx_buf[4]; // Dummy byte + 2 data bytes

    // Prepare the transmit buffer
    tx_buf[0] = 0x80;                   // READ command (0x80)
    tx_buf[1] = (address >> 16) & 0xFF; // Address byte 2 (MSB)
    tx_buf[2] = (address >> 8) & 0xFF;  // Address byte 1
    tx_buf[3] = address & 0xFF;         // Address byte 0 (LSB)
    tx_buf[4] = 0x00;                   // Dummy byte needed for read operation

    // Configure the SPI transaction
    spi_transaction_t trans = {
        .length = 5 * 8,     // Total bits to send (5 bytes)
        .rxlength = 3 * 8,   // Total bits to receive (3 bytes: dummy + 2 data)
        .tx_buffer = tx_buf, // Pointer to the transmit buffer
        .rx_buffer = rx_buf  // Pointer to the receive buffer
    };

    // Perform the SPI transaction
    esp_err_t check_spi_data_16 = spi_device_polling_transmit(spi_handle, &trans);
    ESP_ERROR_CHECK(check_spi_data_16);

    // Combine the received bytes (skip first dummy byte)
    uint16_t data = (rx_buf[1] << 8) | rx_buf[2];
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
    return data;
}

esp_err_t host_command(uint8_t command)
{
    esp_err_t ret;
    spi_transaction_t trans = {
        .length = 8, // 8 bits (1 byte)
        .tx_buffer = &command,
        .rx_buffer = NULL,
    };
    
    // Send the command
    ret = spi_device_polling_transmit(spi_handle, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(SPI_TAG, "SPI transmit failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Some commands require a small delay
    switch (command) {
        case ACTIVE:
        case CLKEXT:
            vTaskDelay(pdMS_TO_TICKS(20));
            break;
        default:
            vTaskDelay(pdMS_TO_TICKS(1));
            break;
    }
    
    return ESP_OK;
}


uint32_t BEGIN(uint8_t prim)
{
    // Validate primitive type (1-9 according to documentation)
    if (prim < 1 || prim > 9)
    {
        prim = 1; // Default to BITMAPS if invalid
    }

    // Format according to spec:
    // 31-24: 0x1F
    // 23-4: Reserved (0)
    // 3-0: Primitive type
    return 0x1F000000 | (prim & 0xF);
}

uint32_t CLEAR_COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue)
{
    return 0x02000000 | (red << 16) | (blue << 8) | green;
}

uint32_t CLEAR(uint8_t c, uint8_t s, uint8_t t)
{
    return 0x26000000 | (c << 2) | (s << 1) | t;
}

uint32_t COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue)
{
    return 0x04000000 | (red << 16) | (blue << 8) | green;
}

uint32_t POINT_SIZE(uint16_t size)
{
    return 0x0D000000 | size;
}

uint32_t DISPLAY()
{
    return 0x00000000;
}
uint32_t END()
{
    return 0x21000000;
}
uint32_t VERTEX2II(uint16_t x, uint16_t y, uint8_t handle, uint8_t cell)
{
    return 0x40000000 | ((x & 0x1FF) << 21) | ((y & 0x1FF) << 12) | ((handle & 0x1F) << 7) | (cell & 0x7F);
}

void ft812_init()
{
    host_command(CLKEXT);
    host_command(ACTIVE); // send host command "ACTIVE" to FT81X
    vTaskDelay(pdMS_TO_TICKS(300)); // Wait for 300ms

    // Configure video time registers
    spi_write_16(REG_HSIZE, 800);  // horizontal resolution
    spi_write_16(REG_HCYCLE, 928); // number of clocks per line
    spi_write_16(REG_HOFFSET, 88); // horizontal offset from starting signal
    spi_write_16(REG_HSYNC0, 0);   // hsync falls after this clock
    spi_write_16(REG_HSYNC1, 48);  // hsync rises after this clock

    spi_write_16(REG_VSIZE, 480);  // vertical resolution
    spi_write_16(REG_VCYCLE, 525); // number of clocks per line
    spi_write_16(REG_VOFFSET, 32); // vertical offset from starting signal
    spi_write_16(REG_VSYNC0, 0);   // vsync falls after this clock
    spi_write_16(REG_VSYNC1, 3);   // vsync rises after this clock

    spi_write_8(REG_SWIZZLE, 0); // no swizzle
    spi_write_8(REG_PCLK_POL, 0);
    spi_write_8(REG_CSPREAD, 0); // spread spectrum on
    spi_write_8(REG_DITHER, 1);  // dithering on

    // write first display list
    spi_write_32(RAM_DL + 0, CLEAR_COLOR_RGB(0, 0, 0));

    spi_write_32(RAM_DL + 4, CLEAR(1, 1, 1));
    spi_write_32(RAM_DL + 8, DISPLAY());

    spi_write_8(REG_DLSWAP, 2); // display list swap
    spi_write_8(REG_GPIO_DIR, 0x80 | spi_read_8(REG_GPIO_DIR));
    spi_write_8(REG_GPIO, 0x080 | spi_read_8(REG_GPIO)); // enable display bit

    spi_write_8(REG_PCLK, 2);                           // after this display is visible on the LCD
}

void display_text()
{
    spi_write_32(RAM_DL + 0, CLEAR(1, 1, 1));                // clear screen
    spi_write_32(RAM_DL + 4, BEGIN(BITMAPS));                // start drawing bitmaps
    spi_write_32(RAM_DL + 8, VERTEX2II(220, 110, 31, 'F'));  // ascii F in font 31
    spi_write_32(RAM_DL + 12, VERTEX2II(244, 110, 31, 'T')); // ascii T
    spi_write_32(RAM_DL + 16, VERTEX2II(270, 110, 31, 'D')); // ascii D
    spi_write_32(RAM_DL + 20, VERTEX2II(299, 110, 31, 'I')); // ascii I
    spi_write_32(RAM_DL + 24, END());
    spi_write_32(RAM_DL + 28, COLOR_RGB(160, 22, 22));    // change colour to red
    spi_write_32(RAM_DL + 32, POINT_SIZE(320));           // set point size to 20 pixels in
    spi_write_32(RAM_DL + 36, BEGIN(POINTS));             // start drawing points
    spi_write_32(RAM_DL + 40, VERTEX2II(192, 133, 0, 0)); // red point
    spi_write_32(RAM_DL + 44, END());
    spi_write_32(RAM_DL + 48, DISPLAY());
}

void app_main()
{
    spi_init(GPIO_SCLK, GPIO_MOSI, GPIO_MISO);
    spi_add_device(10 * 1000 * 1000, 0, 0, 1, GPIO_CS);
    ft812_init();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}


/**
    ft812_init();
    display_texy();

    // Wait for another 20ms if needed

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        uint32_t reg_id = spi_read_32(0x302000);
        printf("REG_ID: 0x%08lX\n", reg_id);
        // Expected output: 0x0000007C (if communication is working)
    }
        */
