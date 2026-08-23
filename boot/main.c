//------------------------------------------------------------------------------

/// @file main.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include <stdio.h>

#include <stm32l4xx.h>

#include <stm32l4xx_hal.h>

#include <FreeRTOS.h>
#include <task.h>

//------------------------------------------------------------------------------

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationTickHook(void)
{
    HAL_IncTick();
}

void vApplicationMallocFailedHook(void)
{
    
}

void vApplicationIdleHook(void)
{
    __WFI();
}


void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{

}

void vAssertCalled(const char *pcFile, unsigned long ulLine)
{

}

//------------------------------------------------------------------------------

void rts_timer_configure(void)
{

}

uint32_t rts_get_time_counter_value(void)
{
    return SysTick->VAL;
}

//------------------------------------------------------------------------------

typedef void(*reset_handler_t)(void);

static void led_task(void *pvParameters)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    while(!(RCC->AHB2ENR & RCC_AHB2ENR_GPIOAEN));

    GPIOA->MODER &= ~GPIO_MODER_MODE5;  // clear mode register for GPIOA pin 5
    GPIOA->MODER |= GPIO_MODER_MODE5_0; // set mode register general puprpose output

    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5);    // set output type  to push-pull
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5_Msk); // disable pull-up and pull-down

    GPIOA->BSRR = (1 << 5); // set bit

    uint32_t blink_counts = 10;

    while (blink_counts--)
    {
        GPIOA->BSRR = 1 << 5;
        vTaskDelay(pdMS_TO_TICKS(100));
        GPIOA->BRR = 1 << 5;
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    __disable_irq();

    SysTick->CTRL = 0;
SysTick->LOAD = 0;
SysTick->VAL = 0;

    reset_handler_t reset_handler_func = (reset_handler_t)*((uint32_t*)(0x8008000 + 4));
    SCB->VTOR = (0x8008000);
    __DSB();
    __ISB();
    __set_CONTROL(0);
    __ISB();
    __set_MSP(*(uint32_t *)0x8008000);

    (reset_handler_func)();
}

//------------------------------------------------------------------------------

int main()
{
    NVIC_SetPriorityGrouping(0x00000003U);
    
    /* SYSCLK = 4 MHz, source: MSI, AHB presc = /1 → HCLK = 4 MHz APB1/APB2 presc = /1 → PCLK1 = PCLK2 = 4 MHz */
    SystemInit();
    SystemCoreClockUpdate();

    HAL_Init();

    xTaskCreate(led_task, "LED", 100, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1);

    return 0;
}

//------------------------------------------------------------------------------
