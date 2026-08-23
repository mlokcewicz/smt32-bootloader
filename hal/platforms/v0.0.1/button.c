//------------------------------------------------------------------------------

/// @file button.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "button.h"

#include <hal.h>

//------------------------------------------------------------------------------

bool button_is_pressed(void)
{
    bool is_pressed = false;

    if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
    {
        is_pressed = true;
        
        while (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
        {

        };
    }

    return is_pressed;
}

//------------------------------------------------------------------------------
