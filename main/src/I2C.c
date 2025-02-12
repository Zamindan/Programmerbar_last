#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_PORT -1
#define CLK_SRC I2C_CLK_SRC_DEFAULT
#define GLITCH_IGNORE_COUNT 7
#define INTERNAL_PULLUP 1

#define DEV_ADDR_LENGTH I2C_ADDR_BIT_LEN_7

#define TAG "I2C"

/**
 * @brief Function for initialising I2C with a bus handle.
 * 
 * @param bus_handle_name Pointer to the bus handle name
 * @param sda_pin Select the SDA GPIO
 * @param scl_pin Select the SCL GPIO
 */
void i2c_init(i2c_master_bus_handle_t bus_handle_name, gpio_num_t sda_pin, gpio_num_t scl_pin){
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = CLK_SRC,
        .i2c_port = I2C_PORT,
        .scl_io_num = sda_pin,
        .sda_io_num = scl_pin,
        .glitch_ignore_cnt = GLITCH_IGNORE_COUNT,
        .flags.enable_internal_pullup = INTERNAL_PULLUP};

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle_name));
};

/**
 * @brief 
 * 
 * @param bus_handle_name 
 * @param dev_handle_name 
 * @param device_address 
 * @param scl_clock_speed 
 */
void i2c_add_device(i2c_master_bus_handle_t bus_handle_name, i2c_master_dev_handle_t dev_handle_name, uint16_t device_address, uint32_t scl_clock_speed){
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = DEV_ADDR_LENGTH,
        .device_address = device_address,
        .scl_speed_hz = scl_clock_speed,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle_name, &dev_cfg, &dev_handle_name));

    esp_err_t check = i2c_master_probe(bus_handle_name, device_address, -1);
    if (check == ESP_OK)
    {
        ESP_LOGI(TAG, "I2C Address found at %d", device_address);
    }
    else
    {
        ESP_LOGI(TAG, "No I2C device found on %d", device_address);
    };
};

void i2c_write(i2c_master_dev_handle_t dev_handle_name, uint8_t register_address, uint16_t data_to_write){
    uint8_t write_buffer[3] = {0};
    write_buffer[0] = register_address;
    write_buffer[1] = (data_to_write >> 8) & 0xFF;
    write_buffer[2] = data_to_write & 0xFF;
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle_name, write_buffer, sizeof(write_buffer), -1));
}

float i2c_read(i2c_master_dev_handle_t dev_handle_name){
    uint16_t read_buffer[1] = {0};
    i2c_master_receive(dev_handle_name, read_buffer, sizeof(read_buffer), -1);
    uint16_t result = read_buffer[0];
    return result;
}

float i2c_write_read(){

}