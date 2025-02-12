#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "I2C TEST"

#define I2C_PORT -1
#define I2C_SDA_GPIO GPIO_NUM_11
#define I2C_SCL_GPIO GPIO_NUM_12
#define CLK_SRC I2C_CLK_SRC_DEFAULT
#define GLITCH_IGNORE_COUNT 7
#define INTR_PRI 1
#define INTERNAL_PULLUP 1

#define DEV_ADDR_LENGTH I2C_ADDR_BIT_LEN_7
#define DEV_ADD 0b1001000 // Address of TMP102
#define SCL_SPEED_HZ 100000
#define SCL_WAIT_US 0

void app_main()
{

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = CLK_SRC,
        .i2c_port = I2C_PORT,
        .scl_io_num = I2C_SCL_GPIO,
        .sda_io_num = I2C_SDA_GPIO,
        .glitch_ignore_cnt = GLITCH_IGNORE_COUNT,
        .flags.enable_internal_pullup = INTERNAL_PULLUP};

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = DEV_ADDR_LENGTH,
        .device_address = DEV_ADD,
        .scl_speed_hz = SCL_SPEED_HZ,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    esp_err_t check = i2c_master_probe(bus_handle, DEV_ADD, -1);
    if (check == ESP_OK)
    {
        ESP_LOGI(TAG, "I2C Address found at %d", DEV_ADD);

        uint8_t I2C_write[1] = {0x00};
        ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, I2C_write, sizeof(I2C_write), -1));

        uint8_t read_I2C[2] = {0};
        while (1)
        {
            i2c_master_receive(dev_handle, read_I2C, sizeof(read_I2C), -1);
            int16_t temp_raw = (read_I2C[0] << 4) | (read_I2C [1] >> 4);
            if (temp_raw & 0x800) // Check if negative
            {
                temp_raw |= 0xF000; // Sign extend if negative
            }
            float temperature = temp_raw * 0.0625;
            ESP_LOGI(TAG, "I2C read value: %.4f°C", temperature);
            vTaskDelay(pdMS_TO_TICKS(1000));
        };
    }
    else
    {
        ESP_LOGI(TAG, "NO I2C Address found");
    };
}