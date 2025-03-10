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

#define BAUD_RATE 115200
#define DATA_BITS UART_DATA_8_BITS
#define PARITY UART_PARITY_DISABLE
#define STOP_BITS UART_STOP_BITS_1
#define FLOW_CTRL UART_HW_FLOWCTRL_CTS_RTS
#define RX_FLOW_CTRL_THRESH 122
#define SOURCE_CLK UART_SCLK_DEFAULT

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
 * 
 * @details This function initialises the UART module with the specified parameters.
 */
void uart_init(uart_port_t uart_port, int tx_gpio, int rx_gpio, int rts_gpio, int cts_gpio, int uart_buffer_size, QueueHandle_t *uart_queue)
{
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = DATA_BITS,
        .parity = PARITY,
        .stop_bits = STOP_BITS,
        .flow_ctrl = FLOW_CTRL,
        .rx_flow_ctrl_thresh = RX_FLOW_CTRL_THRESH,
        .source_clk = SOURCE_CLK,
    };
    ESP_ERROR_CHECK(uart_param_config(uart_port, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(uart_port, tx_gpio, rx_gpio, rts_gpio, cts_gpio));

    ESP_ERROR_CHECK(uart_driver_install(uart_port, uart_buffer_size, uart_buffer_size, 10, uart_queue, 0));
}
