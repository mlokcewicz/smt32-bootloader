//------------------------------------------------------------------------------

/// @file dma.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include <hal.h>

#include "dma.h"

//------------------------------------------------------------------------------

void dma_init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 10, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

//------------------------------------------------------------------------------
