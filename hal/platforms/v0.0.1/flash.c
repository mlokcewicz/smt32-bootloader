//------------------------------------------------------------------------------

/// @file flash.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include "flash.h"

#include <hal.h>

//------------------------------------------------------------------------------

static uint32_t GetPage(uint32_t Addr)
{
    uint32_t page = 0;

    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        /* Bank 1 */
        page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        /* Bank 2 */
        page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}

static uint32_t GetBank(uint32_t Addr)
{
    uint32_t bank = 0;

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
    {
        /* No Bank swap */
        if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_1;
        }
        else
        {
            bank = FLASH_BANK_2;
        }
    }
    else
    {
        /* Bank swap */
        if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_2;
        }
        else
        {
            bank = FLASH_BANK_1;
        }
    }

    return bank;
}

//------------------------------------------------------------------------------

__weak void flash_dword_program_cb(void)
{

}

bool flash_erase(uint32_t start_addr, uint32_t size)
{
    uint32_t end_addr = start_addr + size - 1;
    uint32_t PAGEError = 0;

    HAL_FLASH_Unlock();

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
    /* Get the 1st page to erase */
    uint32_t FirstPage = GetPage(start_addr);
    /* Get the number of pages to erase from 1st page */
    uint32_t NbOfPages = GetPage(end_addr) - FirstPage + 1;
    /* Get the bank */
    uint32_t BankNumber = GetBank(start_addr);

    FLASH_EraseInitTypeDef EraseInitStruct = {0};

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = BankNumber;
    EraseInitStruct.Page = FirstPage;
    EraseInitStruct.NbPages = NbOfPages;

    bool ret = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) == HAL_OK;

    HAL_FLASH_Lock();

    return ret;
}

bool flash_program(uint32_t start_addr, uint64_t data[], uint32_t dwords)
{
    if (start_addr % sizeof(uint64_t))
        return false;

    uint32_t size = dwords * sizeof(uint64_t);
    uint32_t end_addr = start_addr + size;
    bool ret = false;
    
    HAL_FLASH_Unlock();

    while (start_addr < end_addr)
    {
        ret = (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, start_addr, *data) == HAL_OK);

        if (!ret)
            break;

        start_addr = start_addr + 8;
        data++;

        flash_dword_program_cb();
    }

    HAL_FLASH_Lock();

    return ret;
}

//------------------------------------------------------------------------------
