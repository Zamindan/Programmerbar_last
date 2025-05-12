#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include <inttypes.h>

#include "spi.h"
#include "FT812.h"

#define SPI_TAG "SPI"
#define LCD_TAG "LCD"

#define SPI_HOST SPI2_HOST // Use SPI2 (HSPI)
#define SPI_MISO_PIN GPIO_NUM_13
#define SPI_MOSI_PIN GPIO_NUM_11
#define SPI_SCLK_PIN GPIO_NUM_12
#define SPI_CS_PIN GPIO_NUM_10
#define PD_PIN GPIO_NUM_8 // Power Down (PD#) Pin

#define GPIO_LED_RED 37
#define GPIO_LED_GREEN 36

spi_device_handle_t spi_handle;

void ft812_init()
{
    // Turn on the FT812 chip
    gpio_set_direction(PD_PIN, GPIO_MODE_OUTPUT); 
    gpio_set_level(PD_PIN, 0);    
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(PD_PIN, 1);    
    vTaskDelay(pdMS_TO_TICKS(20)); 

    host_command(ACTIVE);          
    vTaskDelay(pdMS_TO_TICKS(300)); 

    
    // Configure video time registers
    spi_write_16(REG_HSIZE, 800);  
    spi_write_16(REG_HCYCLE, 928); 
    spi_write_16(REG_HOFFSET, 88); 
    spi_write_16(REG_HSYNC0, 0);   
    spi_write_16(REG_HSYNC1, 48);  

    spi_write_16(REG_VSIZE, 480);  
    spi_write_16(REG_VCYCLE, 525); 
    spi_write_16(REG_VOFFSET, 32); 
    spi_write_16(REG_VSYNC0, 0);   
    spi_write_16(REG_VSYNC1, 3);   

    spi_write_8(REG_SWIZZLE, 0); 
    spi_write_8(REG_PCLK_POL, 0);
    spi_write_8(REG_CSPREAD, 0); 
    spi_write_8(REG_DITHER, 1);  

 
    spi_write_32(RAM_DL + 0, CLEAR_COLOR_RGB(0, 0, 0));

    spi_write_32(RAM_DL + 4, CLEAR(1, 1, 1));
    spi_write_32(RAM_DL + 8, DISPLAY());

    spi_write_8(REG_DLSWAP, 2); 
    spi_write_8(REG_GPIO_DIR, 0x80 | spi_read_8(REG_GPIO_DIR));
    spi_write_8(REG_GPIO, 0x080 | spi_read_8(REG_GPIO)); 

    spi_write_8(REG_PCLK, 2); 
} 

 

void indicator_active(){
    gpio_set_level(GPIO_LED_RED, 0);   // Turn off red LED
    gpio_set_level(GPIO_LED_GREEN, 1); // Turn on green LED
}

void indicator_standby(){
        gpio_set_level(GPIO_LED_GREEN, 0); // Turn off green LED
        gpio_set_level(GPIO_LED_RED, 1); // Turn on green LED
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for 1 second
        gpio_set_level(GPIO_LED_RED, 0); // Turn off green LED
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for 1 second
    }

void indicator_alert(){
    gpio_set_level(GPIO_LED_RED, 1);   // Turn on red LED
    gpio_set_level(GPIO_LED_GREEN, 0); // Turn off green LED
};


// Function to display text on the screen
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
    spi_init(SPI_SCLK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI2_HOST);
    spi_add_device(10000, 0, 0, 1, SPI_CS_PIN, SPI2_HOST);

    ft812_init();

    gpio_set_direction(GPIO_LED_RED, GPIO_MODE_OUTPUT); // Set PD pin as output
    gpio_set_direction(GPIO_LED_GREEN, GPIO_MODE_OUTPUT); // Set PD pin as output

    // Configure video time registers
    spi_read_16(REG_HSIZE);   
    spi_read_16(REG_HCYCLE);  
    spi_read_16(REG_HOFFSET); 
    spi_read_16(REG_HSYNC0);  
    spi_read_16(REG_HSYNC1);  

    spi_read_16(REG_VSIZE);   
    spi_read_16(REG_VCYCLE);  
    spi_read_16(REG_VOFFSET); 
    spi_read_16(REG_VSYNC0);  
    spi_read_16(REG_VSYNC1);  

    spi_read_8(REG_SWIZZLE); 
    spi_read_8(REG_PCLK_POL);
    spi_read_8(REG_CSPREAD); 
    spi_read_8(REG_DITHER); 

    display_text();

    spi_write_32(REG_PWM_HZ, 1000);  
    spi_write_8(REG_PWM_DUTY, 128);   
    

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for 1 second
    }
}

