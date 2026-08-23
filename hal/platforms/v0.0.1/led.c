//------------------------------------------------------------------------------

/// @file led.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "led.h"

#include <hal.h>

//------------------------------------------------------------------------------

void led_set(bool state)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void led_toggle(void)
{
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

//------------------------------------------------------------------------------
