//------------------------------------------------------------------------------

/// @file hal.h
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#ifndef HAL_H_
#define HAL_H_

//------------------------------------------------------------------------------

#include "stm32l4xx_hal.h"

#include <gpio.h>
#include <iwdg.h>
#include <dma.h>
#include <usart.h>
#include <flash.h>
#include <crc.h>

#include <button.h>
#include <led.h>

//------------------------------------------------------------------------------

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

//------------------------------------------------------------------------------

void Error_Handler(void);

void SystemClock_Config(void);

//------------------------------------------------------------------------------

#endif /* HAL_H_ */
