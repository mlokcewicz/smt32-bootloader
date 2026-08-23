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

//------------------------------------------------------------------------------

bool app_loader_init(void);

bool app_loader_is_dfu_requested(void);

bool app_loader_peform_dfu(void);

void app_loader_jump_to_app(void);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* APP_LOADER_H_ */

//------------------------------------------------------------------------------
