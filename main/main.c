#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include <inttypes.h>

#include "spi.c"
#include "FT812.c"

#define SPI_TAG "SPI"
#define LCD_TAG "LCD"

#define SPI_HOST SPI2_HOST // Use SPI2 (HSPI)
#define GPIO_MISO GPIO_NUM_13
#define GPIO_MOSI GPIO_NUM_11
#define GPIO_SCLK GPIO_NUM_12
#define GPIO_CS GPIO_NUM_10
#define GPIO_PD GPIO_NUM_8 // Power Down (PD#) Pin

#define GPIO_LED_RED 38
#define GPIO_LED_GREEN 36

void ft812_init()
{
    gpio_set_direction(GPIO_PD, GPIO_MODE_OUTPUT); // Set PD pin as output
    gpio_set_level(GPIO_PD, 0);    // Set PD pin low
    vTaskDelay(pdMS_TO_TICKS(20)); // Wait for 20ms
    gpio_set_level(GPIO_PD, 1);    // Set PD pin high
    vTaskDelay(pdMS_TO_TICKS(20)); // Wait for another 20ms

    host_command(ACTIVE);           // send host command "ACTIVE" to FT81X
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
    spi_write_32(RAM_DL + 0, CLEAR_COLOR_RGB(255, 0, 0));

    spi_write_32(RAM_DL + 4, CLEAR(1, 1, 1));
    spi_write_32(RAM_DL + 8, DISPLAY());

    spi_write_8(REG_DLSWAP, 2); // display list swap
    spi_write_8(REG_GPIO_DIR, 0x80 | spi_read_8(REG_GPIO_DIR));
    spi_write_8(REG_GPIO, 0x080 | spi_read_8(REG_GPIO)); // enable display bit

    spi_write_8(REG_PCLK, 2); // after this display is visible on the LCD*/
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
    spi_init(GPIO_SCLK, GPIO_MOSI, GPIO_MISO, SPI2_HOST);
    spi_add_device(100000, 0, 0, 1, GPIO_CS, SPI2_HOST);
    ft812_init();

    // Configure video time registers
    spi_read_16(REG_HSIZE);   // horizontal resolution
    spi_read_16(REG_HCYCLE);  // number of clocks per line
    spi_read_16(REG_HOFFSET); // horizontal offset from starting signal
    spi_read_16(REG_HSYNC0);  // hsync falls after this clock
    spi_read_16(REG_HSYNC1);  // hsync rises after this clock

    spi_read_16(REG_VSIZE);   // vertical resolution
    spi_read_16(REG_VCYCLE);  // number of clocks per line
    spi_read_16(REG_VOFFSET); // vertical offset from starting signal
    spi_read_16(REG_VSYNC0);  // vsync falls after this clock
    spi_read_16(REG_VSYNC1);  // vsync rises after this clock

    spi_read_8(REG_SWIZZLE); // no swizzle
    spi_read_8(REG_PCLK_POL);
    spi_read_8(REG_CSPREAD); // spread spectrum on
    spi_read_8(REG_DITHER);  // dithering on

    vTaskDelay(pdMS_TO_TICKS(10000));
    gpio_set_direction(GPIO_LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_LED_GREEN, GPIO_MODE_OUTPUT);

    // display_text();

    while (1)
    {
        gpio_set_level(GPIO_LED_RED, 1);
        gpio_set_level(GPIO_LED_GREEN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(GPIO_LED_RED, 0);
        gpio_set_level(GPIO_LED_GREEN, 0);
        display_text();
    }
}
