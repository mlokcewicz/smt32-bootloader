//------------------------------------------------------------------------------

/// @file main.c
/// @note Copyright (C) Michał Łokcewicz. All rights reserved.

//------------------------------------------------------------------------------

#include <stdio.h>

#include <stm32l4xx.h>

#include <hal.h>

#include <FreeRTOS.h>
#include <task.h>

#include <data_exchange.h>

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

static void stats_task(void *pvParameters)
{
    uint8_t tasks_quantity = uxTaskGetNumberOfTasks();
    char buf[40 * tasks_quantity]; /* 40 bytes per task */
    HeapStats_t heap_stats;
    TaskStatus_t tasks_info[tasks_quantity];
    uint32_t total_run_time = 0;

    while (1)
    {
        vTaskGetRunTimeStats(buf);
        vPortGetHeapStats(&heap_stats);
        
        uxTaskGetSystemState(tasks_info, tasks_quantity, &total_run_time);

        /* log rts, heap, stack info */

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void led_task(void *pvParameters)
{
    gpio_init();
    iwdg_init();
    uart_init();

    printf("MAIN APP started...\n");

    struct data_exchange_data *data = data_exchange_get_data();
    
    printf("Boot version: %s\n", data->boot_ver);

    bool dfu_req = false;

    led_set(true);

    while (1)
    {
        iwdg_feed();
        led_toggle();
        vTaskDelay(pdMS_TO_TICKS(1000));

        if (button_is_pressed())
        {
            data_exchange_set_dfu_entry_req(dfu_req);

            printf("DFU request: %d\n", dfu_req);

            dfu_req ^= true;
        }
    }
}

//------------------------------------------------------------------------------

int main()
{
    NVIC_SetPriorityGrouping(0x00000003U);
    
    /* SYSCLK = 4 MHz, source: MSI, AHB presc = /1 → HCLK = 4 MHz APB1/APB2 presc = /1 → PCLK1 = PCLK2 = 4 MHz */
    SystemInit();
    SystemCoreClockUpdate();

    HAL_Init();

    SystemClock_Config();

    xTaskCreate(led_task, "LED", 512, NULL, 1, NULL);
    xTaskCreate(stats_task, "STATS", 300, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1);

    return 0;
}

//------------------------------------------------------------------------------
