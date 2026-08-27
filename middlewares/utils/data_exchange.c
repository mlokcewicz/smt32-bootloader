//------------------------------------------------------------------------------

/// @file data_exchange.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "data_exchange.h"

#include <string.h>

//------------------------------------------------------------------------------

#define DATA_EXCHANGE_MAGIC 0xDEADBEEF

//------------------------------------------------------------------------------

struct data_exchange_data
{
    char boot_ver[DATA_EXCHNGE_BOOT_VER_LEN];
    uint32_t boot_ver_magic;
    bool boot_entry_req;
    uint32_t boot_entry_req_magic;
};

//------------------------------------------------------------------------------

__attribute__((section(".data_exchange_section_ram"))) struct data_exchange_data data;

//------------------------------------------------------------------------------

void data_exchange_set_boot_ver(const char* boot_ver)
{   
    strncpy(data.boot_ver, boot_ver, DATA_EXCHNGE_BOOT_VER_LEN);
    data.boot_ver_magic = DATA_EXCHANGE_MAGIC;
}

void data_exchange_set_dfu_entry_req(bool dfu_entry_req)
{
    data.boot_entry_req = dfu_entry_req;
    data.boot_entry_req_magic = DATA_EXCHANGE_MAGIC;
}

const char* data_exchange_get_boot_ver(void)
{
    if (data.boot_ver_magic != DATA_EXCHANGE_MAGIC)
        return "";
    
    return data.boot_ver;
}

bool data_exchange_get_boot_entry_req(void)
{
    if (data.boot_entry_req_magic != DATA_EXCHANGE_MAGIC)
        return NULL;
    
    return data.boot_entry_req;
}

//------------------------------------------------------------------------------
