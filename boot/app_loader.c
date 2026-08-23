//------------------------------------------------------------------------------

/// @file app_loader.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "app_loader.h"

#include <stdio.h>

#include <stm32l4xx.h>

#include <hal.h>

#include <data_exchange.h>

//------------------------------------------------------------------------------

#define DEFAULT_BOOT_VERSION "v1.2.3"

//------------------------------------------------------------------------------

extern const uint8_t __app_start__;
extern const uint8_t __app_size__;

static uintptr_t app_start = (uintptr_t)&__app_start__;
static uintptr_t app_size = (uintptr_t)&__app_size__;

//------------------------------------------------------------------------------

typedef void(*reset_handler_t)(void);

//------------------------------------------------------------------------------

bool app_loader_init(void)
{
    struct data_exchange_data *data = data_exchange_get_data();
    
    printf("APP version: %s, DFU request: %d\n", data->app_ver, data->dfu_entry_req);

    data_exchange_set_boot_ver(DEFAULT_BOOT_VERSION);
    
    return true;
}

bool app_loader_is_dfu_requested(void)
{
    struct data_exchange_data *data = data_exchange_get_data();

    bool requested = button_is_pressed() || (data && data->dfu_entry_req);

    printf("DFU requested: %s\n", requested ? "true" : "false");

    return requested;
}

bool app_loader_perform_dfu(void)
{
    /* TODO: Implement UART data reception and FLASH write */

    iwdg_feed();

    data_exchange_set_dfu_entry_req(false);

    printf("DFU finished successfully\n");

    return true;
}

bool app_loader_app_is_valid(void)
{
    /* TODO: Check app CRC */    
    (void)app_size;

    printf("Application CRC: OK\n");

    return true;
}

void app_loader_jump_to_app(void)
{
    __disable_irq();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    reset_handler_t reset_handler_func = (reset_handler_t)*((uint32_t*)(app_start + 4));
    
    SCB->VTOR = (0x8008000);
    __DSB();
    __ISB();

    __set_CONTROL(0);
    __ISB();

    __set_MSP(*(uint32_t *)app_start);

    reset_handler_func();
}

//------------------------------------------------------------------------------
