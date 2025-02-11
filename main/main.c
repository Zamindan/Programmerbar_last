#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"

#define I2C_PORT -1
#define I2C_SDA_GPIO GPIO_NUM_11
#define I2C_SCL_GPIO GPIO_NUM_12
#define CLK_SRC I2C_CLK_SRC_DEFAULT
#define GLITCH_IGNORE_COUNT 7
#define INTR_PRI 1
#define INTERNAL_PULLUP 1

#define DEV_ADDR_LENGTH I2C_ADDR_BIT_LEN_10
#define DEV_ADD 0x28
#define SCL_SPEED_HZ 100000
#define SCL_WAIT_US 0

void app_main(){

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = CLK_SRC,
        .i2c_port = I2C_PORT,
        .scl_io_num = I2C_SCL_GPIO,
        .sda_io_num = I2C_SDA_GPIO,
        .glitch_ignore_cnt = GLITCH_IGNORE_COUNT,
        .flags.enable_internal_pullup = INTERNAL_PULLUP
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}