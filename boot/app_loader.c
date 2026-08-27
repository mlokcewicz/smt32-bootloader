//------------------------------------------------------------------------------

/// @file app_loader.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "app_loader.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include <stm32l4xx.h>

#include <hal.h>

#include <data_exchange.h>

//------------------------------------------------------------------------------

#define DEFAULT_BOOT_VERSION "v1.2.3"

#define DFU_TIMEOUT_MS 300000 /* 5 minutes */
#define DFU_BUFFER_SIZE (16 * 1024)

_Static_assert (!(DFU_BUFFER_SIZE % sizeof(uint64_t)), "DFU buffer size must be multiplicity of DWORD");
_Static_assert (!(sizeof(struct app_loader_image_header) % sizeof(uint64_t)), "APP HEADER size buffer must be multiplicity of DWORD");

//------------------------------------------------------------------------------

struct app_obj
{
    uint32_t addr;
    uint32_t max_size;
};

//------------------------------------------------------------------------------

extern const uint8_t __rom_size__;
extern const uint8_t __boot_start__;
extern const uint8_t __app_start__;
extern const uint8_t __app_size__;

static uintptr_t boot_start = (uintptr_t)&__boot_start__;
static uintptr_t rom_size = (uintptr_t)&__rom_size__;
static uintptr_t app_start = (uintptr_t)&__app_start__;
static uintptr_t max_app_size = (uintptr_t)&__app_size__;

static struct app_obj apps[APP_LOADER_APP_TYPE_MAX];

__attribute__((section(".app_header"))) const volatile struct app_loader_image_header app_header;

__aligned(sizeof(uint64_t)) static uint8_t app_loader_buf[DFU_BUFFER_SIZE];
__aligned(sizeof(uint64_t)) static uint64_t app_header_buf[(sizeof(app_header) + sizeof(uint64_t) - 1) / sizeof(uint64_t)];

static volatile uint32_t buf_tail;
static volatile uint32_t buf_head;

static volatile uint32_t last_rx_tick;

static TaskHandle_t dfu_task_handle;

//------------------------------------------------------------------------------

typedef void(*reset_handler_t)(void);

//------------------------------------------------------------------------------

static bool validate_app_header(const volatile struct app_loader_image_header *header)
{
    if (header->magic != APP_LOADER_APP_HEADER_MAGIC)
    {
        printf("APP HEADER validation error: invalid magic: %ld\n", header->magic);   
        return false;
    }
    
    if (header->app_type >= APP_LOADER_APP_TYPE_MAX)
    {
        printf("APP HEADER validation error: invalid app type: %ld\n", header->app_type);   
        return false;
    }
    
    if (header->app_size > apps[header->app_type].max_size)
    {
        printf("APP HEADER validation error: size (%ld) exceeded for app %ld (max: %ld)\n", header->app_size, header->app_type,  apps[header->app_type].max_size);;   
        return false;
    }

    if (header->app_size == 0)
    {
        printf("APP HEADER validation error: invalid app size: %ld\n", header->app_size);   
        return false;
    }

    printf("APP HEADER OK. App type: %ld, size: %ld\n", header->app_type, header->app_size);

    return true;
}

bool validate_app_crc(uint32_t app_start_addr, uint32_t size, uint32_t expected_crc)
{
    /* TODO */

    return true;
}


static bool get_dfu_timeout_elapsed(void)
{
    if ((xTaskGetTickCount() - last_rx_tick) > pdMS_TO_TICKS(DFU_TIMEOUT_MS))
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

    last_rx_tick = xTaskGetTickCountFromISR();

    BaseType_t higher_priority_task_woken = pdFALSE;

    if (dfu_task_handle != NULL)
    {
        vTaskNotifyGiveFromISR(dfu_task_handle,&higher_priority_task_woken);

        portYIELD_FROM_ISR(higher_priority_task_woken);
    }

}

void uart_rx_err_cb(void)
{
    printf("UART RX error\n");
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
    
    printf("APP version: %s, DFU request: %d\n", app_header.app_version, data->dfu_entry_req);

    data_exchange_set_boot_ver(DEFAULT_BOOT_VERSION);

    apps[APP_LOADER_APP_TYPE_BOOT].addr = (uint32_t)boot_start;
    apps[APP_LOADER_APP_TYPE_BOOT].max_size = rom_size;
    apps[APP_LOADER_APP_TYPE_MAIN].addr = (uint32_t)app_start;
    apps[APP_LOADER_APP_TYPE_MAIN].max_size = max_app_size;
    
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
    buf_head = 0;
    buf_tail = 0;
    last_rx_tick = xTaskGetTickCount();
    dfu_task_handle = xTaskGetCurrentTaskHandle();

    uart_start_rx(app_loader_buf, DFU_BUFFER_SIZE);

    iwdg_feed();

    /* Initial values for application */
    uint32_t received_app_size = apps[APP_LOADER_APP_TYPE_MAIN].max_size;
    uint32_t curr_flash_address = apps[APP_LOADER_APP_TYPE_MAIN].addr;
    uint32_t end_address = curr_flash_address + received_app_size;
    uint64_t last_word_buf = 0xFFFFFFFFFFFFFFFF;
    struct app_loader_image_header* header = (struct app_loader_image_header*)app_header_buf;
    uint16_t header_bytes_received = 0;

    while (curr_flash_address < end_address)
    {
        ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(100));

        iwdg_feed();

        if (get_dfu_timeout_elapsed())
            return false;

        uint32_t temp_head = (buf_head >= buf_tail) ? buf_head : DFU_BUFFER_SIZE; // Prevent buffer flip
        uint32_t bytes_to_write = temp_head - buf_tail;
        uint32_t bytes_left = end_address - curr_flash_address;
        bytes_to_write = bytes_to_write < bytes_left ? bytes_to_write : bytes_left;
        uint32_t dwords_to_write = bytes_to_write / sizeof(uint64_t);
        uint32_t bytes_consumed = dwords_to_write * sizeof(uint64_t);
        uint8_t *write_ptr = &app_loader_buf[buf_tail];
        
        /* Wait for header */
        if (header_bytes_received < sizeof(app_header))
        {
            uint16_t bytes_left_to_copy = sizeof(app_header) - header_bytes_received;
            uint16_t header_bytes_available = bytes_to_write <= bytes_left_to_copy ? bytes_to_write : bytes_left_to_copy;
            
            memcpy((uint8_t*)app_header_buf + header_bytes_received, write_ptr, header_bytes_available);

            header_bytes_received += header_bytes_available;
            buf_tail = (buf_tail + header_bytes_available) % DFU_BUFFER_SIZE;

            if (header_bytes_received < sizeof(app_header))
                continue;

            if (!validate_app_header((void*)app_header_buf))
                return false;

            /* Optionally - check application version */

            received_app_size = header->app_size;
            curr_flash_address = apps[header->app_type].addr;
            end_address = curr_flash_address + received_app_size;

            TickType_t erase_start_tick = xTaskGetTickCount();
            bool erase_ok = flash_erase(curr_flash_address, received_app_size);
            TickType_t erase_time_ticks = xTaskGetTickCount() - erase_start_tick;

            printf("Flash erase time: %lu ms\n", (unsigned long)(erase_time_ticks * portTICK_PERIOD_MS));

            if (!erase_ok)
            {
                printf("Flash erase error\n");
                return false;
            }

            printf("READY\n");

            xTaskNotifyGive(dfu_task_handle);
            continue;
        }

        /* Handle last non-full word */
        if (bytes_left < sizeof(uint64_t) && bytes_left <= bytes_to_write)
        {
            dwords_to_write = 1;
            bytes_consumed = bytes_left;
            memcpy(&last_word_buf, app_loader_buf + buf_tail, bytes_left);
            write_ptr = (uint8_t *)&last_word_buf;
        }

        if (dwords_to_write)
        {
            if (!flash_program(curr_flash_address, (uint64_t*)write_ptr, dwords_to_write))
            {
                printf("Flash program error\n");
                return false;
            }

            curr_flash_address += dwords_to_write * sizeof(uint64_t);
            buf_tail = (buf_tail + bytes_consumed) % DFU_BUFFER_SIZE;
        }
    }

    iwdg_feed();

    if (!validate_app_crc(apps[header->app_type].addr, header->app_size, header->app_crc))
    {
        printf("CRC validation failed after flash\n");
        return false;
    }

    /* Flash app header */
    if (header->app_type != APP_LOADER_APP_TYPE_BOOT)
    {
        if (!flash_erase((uint32_t)&app_header, sizeof(app_header)))
        {
            printf("Flash erase for app header error\n");
            return false;
        }
    
        if (!flash_program((uint32_t)&app_header, app_header_buf, sizeof(app_header_buf) / sizeof(uint64_t)))
        {
            printf("Flash program for app header error\n");
            return false;
        }
    }

    data_exchange_set_dfu_entry_req(false);

    printf("DFU finished successfully\n");

    return true;
}

bool app_loader_app_is_valid(void)
{
    if (!validate_app_header(&app_header))
        return false;

    if (!validate_app_crc(apps[app_header.app_type].addr, app_header.app_size, app_header.app_crc))
        return false;

    printf("Application CRC: OK\n");

    return true;
}

void app_loader_jump_to_app(void)
{
    uart_deinit();
    dma_deinit();
    crc_deinit();

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
