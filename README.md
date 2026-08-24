# stm32-bootloader

Simple bootloader for ARM Cortex-M4 based STM32 microcontrollers

## Capabilities

* DFU over UART (based on DMA in circular mode)
* Application CRC check
* RAM code execution to allow self-upgrade
* SHared memory used to exchange information with main application
   * To application:
      * Bootloader version
   * To bootloader:
      * Application version
      * Application CRC
      * Bootloader entry request

## DFU process

* UART prameters: `115200 baud`, `8N1`, no flow control, `MCU TX` - **PA2**, `MCU RX` - **PA3**
* Communication protocol: 

## Tools

* CMake 3.20.0
* Ninja 1.11.1 
* Arm GNU Toolchain 13.2.rel1 (Build arm-13.7) 13.2.1 20231009
* SEGGER J-Link Software V7.96k

## External libraries

* CMSIS_5 5.9.0
* cmsis_device_l4 v1.7.2
* FreeRTOS-Kernel V10.4.3
* stm32l4xx_hal_driver v1.13.4 (optional)

## Build

### Generate ninja files
`cmake --preset <hw_version>_<config>`

### Build project
`cmake --build --preset <hw_version>_<config>`
