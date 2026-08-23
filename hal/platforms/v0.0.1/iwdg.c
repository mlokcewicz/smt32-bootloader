//------------------------------------------------------------------------------

/// @file iwdg.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "iwdg.h"

//------------------------------------------------------------------------------

IWDG_HandleTypeDef hiwdg;

//------------------------------------------------------------------------------

void iwdg_init(void)
{
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
}

//------------------------------------------------------------------------------
