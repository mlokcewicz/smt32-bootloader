//------------------------------------------------------------------------------

/// @file data_exchange.h
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#ifndef DATA_EXCHANGE_H_
#define DATA_EXCHANGE_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

#include <stdbool.h>

//------------------------------------------------------------------------------

#define DATA_EXCHNGE_BOOT_VER_LEN 32
#define DATA_EXCHNGE_APP_VER_LEN 32

//------------------------------------------------------------------------------

struct data_exchange_data
{
    char boot_ver[DATA_EXCHNGE_BOOT_VER_LEN];
    char app_ver[DATA_EXCHNGE_APP_VER_LEN];
    bool dfu_entry_req;
};

//------------------------------------------------------------------------------

/// @brief Stores the bootloader version in the shared data exchange area.
/// @param boot_ver Pointer to the bootloader version string. Must not be NULL.
void data_exchange_set_boot_ver(const char *boot_ver);

/// @brief Stores the application version in the shared data exchange area.
/// @param app_ver Pointer to the application version string. Must not be NULL.
void data_exchange_set_app_ver(const char *app_ver);

/// @brief Sets or clears the request to enter DFU mode after reset.
/// @param dfu_entry_req Set to true to request DFU mode, or false to clear the request.
void data_exchange_set_dfu_entry_req(bool dfu_entry_req);

/// @brief Returns the data shared between the bootloader and the application.
/// @return Pointer to the data exchange structure located in the shared NOLOAD RAM area.
/// @note The returned memory is not initialized by startup code and may retain its
///       contents across a software reset.
struct data_exchange_data *data_exchange_get_data(void);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* DATA_EXCHANGE_H_ */

//------------------------------------------------------------------------------
