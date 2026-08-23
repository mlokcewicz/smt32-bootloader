//------------------------------------------------------------------------------

/// @file button.h
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#ifndef BUTTON_H_
#define BUTTON_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

#include <stdbool.h>

//------------------------------------------------------------------------------

/// @brief Checks if button is pressed
/// @return true if button is pressed
bool button_is_pressed(void);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H_ */

//------------------------------------------------------------------------------
