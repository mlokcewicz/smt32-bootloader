//------------------------------------------------------------------------------

/// @file crc.h
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#ifndef CRC_H_
#define CRC_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------

/// @brief Initializes the CRC peripheral using the default CRC-32 polynomial
///        and initial value.
void crc_init(void);

/// @brief Calculates the CRC-32 checksum of a data buffer.
/// @param data Pointer to the input data buffer. Must not be NULL when len is non-zero.
/// @param len Length of the input data in bytes.
/// @return Calculated CRC-32 checksum.
uint32_t crc_calculate(uint8_t *data, uint32_t len);

/// @brief Deinitializes the CRC peripheral and disables its clock.
void crc_deinit(void);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* CRC_H_ */

//------------------------------------------------------------------------------
