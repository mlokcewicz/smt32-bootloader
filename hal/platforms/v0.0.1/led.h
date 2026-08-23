//------------------------------------------------------------------------------

/// @file led.h
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#ifndef LED_H_
#define LED_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

#include <stdbool.h>

//------------------------------------------------------------------------------

/// @brief Sets the LED state.
/// @param state true to turn the LED on, false to turn it off.
void led_set(bool state);

/// @brief Toggles the current LED state.
void led_toggle(void);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* LED_H_ */

//------------------------------------------------------------------------------
