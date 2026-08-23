//------------------------------------------------------------------------------

/// @file app_loader.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "app_loader.h"

#include <stm32l4xx.h>

//------------------------------------------------------------------------------

typedef void(*reset_handler_t)(void);

bool app_loader_init(void)
{
    return true;
}

bool app_loader_is_dfu_requested(void)
{
    return false;
}

bool app_loader_peform_dfu(void)
{
    return true;
}

void app_loader_jump_to_app(void)
{
    __disable_irq();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    reset_handler_t reset_handler_func = (reset_handler_t)*((uint32_t*)(0x8008000 + 4));
    SCB->VTOR = (0x8008000);
    __DSB();
    __ISB();
    __set_CONTROL(0);
    __ISB();
    __set_MSP(*(uint32_t *)0x8008000);

    (reset_handler_func)();
}

//------------------------------------------------------------------------------
