//------------------------------------------------------------------------------

/// @file app_loader.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "app_loader.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include <stm32l4xx.h>

#include <hal.h>

#include <data_exchange.h>

//------------------------------------------------------------------------------

#define DEFAULT_BOOT_VERSION "v1.2.3"

#define DFU_TIMEOUT_MS 300000 /* 5 minutes */
#define DFU_BUFFER_SIZE (8 * 1024)

_Static_assert (!(DFU_BUFFER_SIZE % sizeof(uint64_t)), "DFU buffer must be multiplicity of DWORD");

//------------------------------------------------------------------------------

extern const uint8_t __app_start__;
extern const uint8_t __app_size__;

static uintptr_t app_start = (uintptr_t)&__app_start__;
static uintptr_t app_size = (uintptr_t)&__app_size__;

__aligned(sizeof(uint64_t)) static uint8_t app_loader_buf[DFU_BUFFER_SIZE];

static volatile uint32_t buf_tail;
static volatile uint32_t buf_head;

static volatile uint32_t last_rx_tick;

static TaskHandle_t dfu_task_handle;

//------------------------------------------------------------------------------

typedef void(*reset_handler_t)(void);

//------------------------------------------------------------------------------

static bool get_dfu_timeout_elapsed(void)
{
    if ((xTaskGetTickCount() - last_rx_tick) > DFU_TIMEOUT_MS)
    {
        printf("DFU receive timeout\n");
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

void uart_rx_idle_cb(uint16_t size)
{
    buf_head = size;
    buf_head %= DFU_BUFFER_SIZE;

    last_rx_tick = xTaskGetTickCount();

    BaseType_t higher_priority_task_woken = pdFALSE;

    if (dfu_task_handle != NULL)
    {
        vTaskNotifyGiveFromISR(dfu_task_handle,&higher_priority_task_woken);

        portYIELD_FROM_ISR(higher_priority_task_woken);
    }

}

void uart_rx_err_cb(void)
{

}

void flash_dword_program_cb(void)
{
    iwdg_feed();
    led_toggle();
}

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

    if (received_app_size == 0 || received_app_size > app_size)
    {
        printf("Invalid application size\n");
        return false;
    }

    buf_head = 0;
    buf_tail = 0;
    last_rx_tick = xTaskGetTickCount();
    dfu_task_handle = xTaskGetCurrentTaskHandle();

    uart_start_rx(app_loader_buf, DFU_BUFFER_SIZE);

    iwdg_feed();

    if (!flash_erase(app_start, received_app_size))
    {
        printf("Flash erase error\n");
        return false;
    }

    uint32_t curr_flash_address = app_start;
    uint32_t end_address = app_start + received_app_size;
    uint64_t last_word_buf = 0xFFFFFFFFFFFFFFFF;

    while (curr_flash_address < end_address)
    {
        uint32_t temp_head = (buf_head >= buf_tail) ? buf_head : DFU_BUFFER_SIZE; // Prevent buffer flip
        uint32_t bytes_to_write = temp_head - buf_tail;
        uint32_t dwords_to_write = bytes_to_write / sizeof(uint64_t);
        uint32_t bytes_left = end_address - curr_flash_address;
        uint64_t *write_ptr = (uint64_t*)&app_loader_buf[buf_tail];
        
        /* Handle last non-full word */
        if (bytes_left < sizeof(uint64_t) && bytes_left <= bytes_to_write)
        {
            dwords_to_write = 1;
            memcpy(&last_word_buf, app_loader_buf + buf_tail, bytes_left);
            write_ptr = &last_word_buf;
        }

        if (dwords_to_write)
        {
            if (!flash_program(curr_flash_address, write_ptr, dwords_to_write))
            {
                printf("Flash program error\n");
                return false;
            }

            curr_flash_address += dwords_to_write * sizeof(uint64_t);
            buf_tail = (buf_tail + dwords_to_write * sizeof(uint64_t)) % DFU_BUFFER_SIZE;

            continue;
        }

        iwdg_feed();

        if (get_dfu_timeout_elapsed())
            return false;

        ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS(100));
    }

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
    uart_deinit();
    dma_deinit();

    __disable_irq();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    reset_handler_t reset_handler_func = (reset_handler_t)*((uint32_t*)(app_start + 4));
    
    SCB->VTOR = app_start;
    __DSB();
    __ISB();

    __set_CONTROL(0);
    __ISB();

    __set_MSP(*(uint32_t *)app_start);

    __enable_irq();

    reset_handler_func();
}

//------------------------------------------------------------------------------
