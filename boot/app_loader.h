//------------------------------------------------------------------------------

/// @file app_loader.h
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#ifndef APP_LOADER_H_
#define APP_LOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------

#define APP_LOADER_APP_HEADER_MAGIC 0xDEADBEEF

//------------------------------------------------------------------------------

enum app_loader_app_type
{
    APP_LOADER_APP_TYPE_BOOT,
    APP_LOADER_APP_TYPE_MAIN,

    APP_LOADER_APP_TYPE_MAX,
};

//------------------------------------------------------------------------------

struct app_loader_image_header
{
    uint32_t magic;
    uint32_t app_type;
    uint32_t app_size;
    uint32_t app_crc;
    char app_version[16];
} __packed;

//------------------------------------------------------------------------------

/// @brief Initializes the application loader.
/// @return true if initialization succeeds, otherwise false.
bool app_loader_init(void);

/// @brief Checks whether entering DFU mode has been requested.
/// @return true if a DFU request is pending, otherwise false.
bool app_loader_is_dfu_requested(void);

/// @brief Performs the device firmware update procedure.
/// @return true if the update succeeds, otherwise false.
bool app_loader_perform_dfu(void);

/// @brief Checks application CRC
/// @return true if application is valid, otherwise false. 
bool app_loader_app_is_valid(void);

/// @brief Stops bootloader activity and transfers execution to the application.
/// @note A valid application image and vector table must be present at APP_START.
///       This function is not expected to return after a successful jump.
void app_loader_jump_to_app(void);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* APP_LOADER_H_ */

//------------------------------------------------------------------------------
