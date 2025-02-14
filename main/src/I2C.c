#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"

/**
 * @file I2C.c
 * @brief Functions for basic I2C functionaility.
 * 
 * These functions are created to make it easier to use the I2C
 * functionality of the ESP32-S3, specifically it has been created
 * with the INA237 IC in mind.
 * The file contains the source code for I2C initilisation, device addition and data transmission functions.
 * 
 * @author Sondre Pettersen
 * @date 2025-02-13
 */

#define I2C_PORT -1
#define CLK_SRC I2C_CLK_SRC_DEFAULT
#define GLITCH_IGNORE_COUNT 7
#define INTERNAL_PULLUP 1

#define DEV_ADDR_LENGTH I2C_ADDR_BIT_LEN_7

#define TAG "I2C"

/**
 * @brief Function for initialising I2C with a bus handle.
 * 
 * @param bus_handle_name Pointer to the bus handle name.
 * @param sda_pin Select the SDA GPIO.
 * @param scl_pin Select the SCL GPIO.
 * 
 * @details This function initialises the I2C bus with the specified SDA and SCL pins.
 * @details The function will print a message to the console if the initialisation was successful.
 * @details The function will also print a message to the console if the I2C device was found.
 */
void i2c_init(i2c_master_bus_handle_t *bus_handle_name, gpio_num_t sda_pin, gpio_num_t scl_pin){
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = CLK_SRC,
        .i2c_port = I2C_PORT,
        .scl_io_num = sda_pin,
        .sda_io_num = scl_pin,
        .glitch_ignore_cnt = GLITCH_IGNORE_COUNT,
        .flags.enable_internal_pullup = INTERNAL_PULLUP};

    //Initialise the I2C master bus with the above config.
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, bus_handle_name));
};

/**
 * @brief Function for adding an I2C device to the bus.
 * 
 * @param bus_handle_name Handle to the I2C bus.
 * @param dev_handle_name Handle to the I2C device.
 * @param device_address Address of the I2C device.
 * @param scl_clock_speed Clock speed for the I2C bus.
 * 
 * @details This function adds an I2C device to the bus with the specified address and clock speed.
 * @details The function will print a message to the console if the I2C device was found.
 * @details The function will also print a message to the console if the I2C device was not found.
 */
void i2c_add_device(i2c_master_bus_handle_t bus_handle_name, i2c_master_dev_handle_t *dev_handle_name, uint16_t device_address, uint32_t scl_clock_speed){
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = DEV_ADDR_LENGTH,
        .device_address = device_address,
        .scl_speed_hz = scl_clock_speed,
    };

    // Add the I2C device to the bus.
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle_name, &dev_cfg, dev_handle_name));

    // Probe the I2C device to check if it is present.
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


/**
 * @brief Function for writing data to an I2C device.
 * 
 * @param dev_handle_name Handle to the I2C device.
 * @param register_address Address of the register to write to.
 * @param data_to_write Data to write to the register.
 * 
 * @details This function writes data to the specified register of the I2C device..
 */
void i2c_write(i2c_master_dev_handle_t dev_handle_name, uint8_t register_address, uint16_t data_to_write){
    uint8_t write_buffer[3] = {0};
    write_buffer[0] = register_address; // Set the register address.
    write_buffer[1] = (data_to_write >> 8) & 0xFF;  // Set the high byte of the data.
    write_buffer[2] = data_to_write & 0xFF; // Set the low byte of the data.

    // Transmit the data to the I2C device.
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle_name, write_buffer, sizeof(write_buffer), -1));
}

/**
 * @brief Function for setting the register pointer of an I2C device.
 * 
 * @param dev_handle_name Handle of the I2C device.
 * @param register_address Address of the register pointer
 * 
 * @details This function sets the register pointer of the I2C device to the specified register.
 */
void i2c_set_register_pointer(i2c_master_dev_handle_t dev_handle_name, uint8_t register_address){
    // Transmit the register address to set the register pointer.
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle_name, &register_address, sizeof(register_address), -1));
}

/**
 * @brief Function for reading data from an I2C device from a specific register.
 * 
 * @param dev_handle_name Handle to the I2C device.
 * @param register_address Address of the register to read from
 * @return float Returns the data read from the register
 * 
 * @details This function reads data from the specified register of the I2C device.
 * @details The function will return the read data.
 */
float i2c_read(i2c_master_dev_handle_t dev_handle_name, uint8_t register_address){
    // Set the register pointer to a desired register to read from.
    i2c_set_register_pointer(dev_handle_name, register_address);

    // Buffer to hold the read data.
    uint16_t read_buffer[1] = {0};

    // Receive the data from the I2C device
    i2c_master_receive(dev_handle_name, read_buffer, sizeof(read_buffer), -1);

    // Combine the read data into a single value
    float result = read_buffer[0];
    return result;
}

