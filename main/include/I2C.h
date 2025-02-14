#ifndef I2C_H
#define I2C_h

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @file I2C.h
 * @brief Header file for I2C functions.
 * 
 * This file contains the declarations for I2C initialization, device addition, 
 * and data transmission functions.
 * 
 * @author Sondre Pettersen
 * @date 2025-02-13
 */


/**
 * @brief Function for initialising I2C with a bus handle.
 * 
 * @param bus_handle_name Pointer to the bus handle name.
 * @param sda_pin Select the SDA GPIO.
 * @param scl_pin Select the SCL GPIO.
 */
void i2c_init(i2c_master_bus_handle_t *bus_handle_name,
              gpio_num_t sda_pin,
              gpio_num_t scl_pin);

/**
 * @brief Function for adding an I2C device to the bus.
 * 
 * @param bus_handle_name Handle to the I2C bus.
 * @param dev_handle_name Handle to the I2C device.
 * @param device_address Address of the I2C device.
 * @param scl_clock_speed Clock speed for the I2C bus.
 */
void i2c_add_device(i2c_master_bus_handle_t bus_handle_name,
                    i2c_master_dev_handle_t *dev_handle_name,
                    uint16_t device_address,
                    uint32_t scl_clock_speed);

/**
 * @brief Function for writing data to an I2C device.
 * 
 * @param dev_handle_name Handle to the I2C device.
 * @param register_address Address of the register to write to.
 * @param data_to_write Data to write to the register.
 */
void i2c_write(i2c_master_dev_handle_t dev_handle_name,
               uint8_t register_address,
               uint16_t data_to_write);

/**
 * @brief Function for setting the register pointer of an I2C device.
 * 
 * @param dev_handle_name Handle of the I2C device.
 * @param register_address Address of the register pointer
 */
void i2c_set_register_pointer(i2c_master_dev_handle_t dev_handle_name,
                              uint8_t register_address);

/**
 * @brief Function for reading data from an I2C device from a specific register.
 * 
 * @param dev_handle_name Handle to the I2C device.
 * @param register_address Address of the register to read from
 * @return float Returns the data read from the register
 */
float i2c_read(i2c_master_dev_handle_t dev_handle_name,
               uint8_t register_address);

#endif