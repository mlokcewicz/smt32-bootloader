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
#include <stdint.h>

//------------------------------------------------------------------------------

#define DATA_EXCHNGE_BOOT_VER_LEN 32

//------------------------------------------------------------------------------

/// @brief Stores the bootloader version in the shared data exchange area.
/// @param boot_ver Pointer to the bootloader version string. Must not be NULL.
void data_exchange_set_boot_ver(const char *boot_ver);

/// @brief Sets or clears the request to enter DFU mode after reset.
/// @param dfu_entry_req Set to true to request DFU mode, or false to clear the request.
void data_exchange_set_dfu_entry_req(bool dfu_entry_req);

/// @brief Returns the bootloader version stored in the shared data exchange area.
/// @return Pointer to the bootloader version string, or NULL if the stored data is invalid.
const char *data_exchange_get_boot_ver(void);

/// @brief Checks whether entering DFU mode after reset has been requested.
/// @return true if a valid DFU entry request is set, otherwise false.
bool data_exchange_get_boot_entry_req(void);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* DATA_EXCHANGE_H_ */

//------------------------------------------------------------------------------
