//------------------------------------------------------------------------------

/// @file usart.h
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#ifndef USART_H_
#define USART_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

#include <hal.h>

#include <stdbool.h>

//------------------------------------------------------------------------------

extern UART_HandleTypeDef huart2;

//------------------------------------------------------------------------------

/// @brief Initializes UART
void uart_init(void);

/// @brief Starts UART async reception to given buffer
/// @param buf - buffer pointer
/// @param len - buffer length in bytes
void uart_start_rx(uint8_t *buf, uint32_t len);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* USART_H_ */

//------------------------------------------------------------------------------
