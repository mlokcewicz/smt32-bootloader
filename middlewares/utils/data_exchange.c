//------------------------------------------------------------------------------

/// @file data_exchange.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "data_exchange.h"

#include <string.h>

//------------------------------------------------------------------------------

__attribute__((section(".data_exchange_section_ram"))) struct data_exchange_data data;

//------------------------------------------------------------------------------

void data_exchange_set_boot_ver(const char* boot_ver)
{   
    strncpy(data.boot_ver, boot_ver, DATA_EXCHNGE_BOOT_VER_LEN);
}

void data_exchange_set_app_ver(const char* app_ver)
{
    strncpy(data.app_ver, app_ver, DATA_EXCHNGE_APP_VER_LEN);
}

void data_exchange_set_dfu_entry_req(bool dfu_entry_req)
{
    data.dfu_entry_req = dfu_entry_req;
}

struct data_exchange_data *data_exchange_get_data(void)
{
    return &data;
}

//------------------------------------------------------------------------------
