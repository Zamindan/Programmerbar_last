#ifndef UART_H
#define UART_H

#include <stdio.h>
#include "driver/uart.h"
#include "esp_log.h"

/**
 * @file uart.c
 *
 * @brief Implementation of UART initialisation.
 *
 * @author Sondre Pettersen
 * @date 2025.02.13
 *
 */

/**
 * @brief Function to initialise UART.
 * 
 * @param uart_port Selects UART module.
 * @param tx_gpio Specifies TX gpio.
 * @param rx_gpio Specifies RX gpio.
 * @param rts_gpio Specifies RTS gpio.
 * @param cts_gpio Specifies CTS gpio
 * @param uart_buffer_size Defines UART buffer size.
 * @param uart_queue Pointer to the Queue Handle.
 */
void uart_init(uart_port_t uart_port,
               int tx_gpio,
               int rx_gpio,
               int rts_gpio,
               int cts_gpio,
               int uart_buffer_size,
               QueueHandle_t *uart_queue);

#endif