//------------------------------------------------------------------------------

/// @file crc.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "crc.h"

#include <stm32l4xx.h>

#include <hal.h>

//------------------------------------------------------------------------------

static CRC_HandleTypeDef hcrc;

//------------------------------------------------------------------------------

void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc_)
{
  if (hcrc_->Instance==CRC)
  {
    __HAL_RCC_CRC_CLK_ENABLE();
  }

}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef* hcrc_)
{
  if (hcrc_->Instance==CRC)
  {
    __HAL_RCC_CRC_CLK_DISABLE();
  }
}

//------------------------------------------------------------------------------

void crc_init(void)
{

  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
}

void crc_deinit(void)
{
    HAL_CRC_DeInit(&hcrc);

    __HAL_RCC_CRC_CLK_DISABLE();
}

//------------------------------------------------------------------------------
