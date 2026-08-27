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
* Image byte order: little-endian

The host first sends the 32-byte image header. The bootloader validates the header, erases the target Flash area and responds with `READY\n`. After receiving `READY`, the host sends exactly `app_size` payload bytes. This handshake prevents the circular DMA buffer from overflowing while Flash is being erased.

### Image header

| Offset | Size | Field | Description |
|---:|---:|---|---|
| `0x00` | 4 B | `magic` | Image marker, currently `0xDEADBEEF` |
| `0x04` | 4 B | `app_type` | Target image: `0` = bootloader, `1` = main application |
| `0x08` | 4 B | `app_size` | Payload size in bytes, excluding the header |
| `0x0C` | 4 B | `app_crc` | CRC-32 calculated over the payload |
| `0x10` | 16 B | `app_version` | Null-padded ASCII application version, for example `v1.2.3` |

The complete update image has the following layout:

```text
[32-byte header][app_size bytes of payload]
```

The payload is a flat representation of the target Flash range. Any gaps between ELF sections must be filled with `0xFF`, which represents erased Flash. The project therefore generates binary files with `objcopy --gap-fill 0xFF`. This ensures that an ELF programmed directly with J-Link and the corresponding binary programmed through DFU have identical contents and produce the same CRC-32 value.

## Tools

* CMake 3.20.0
* Ninja 1.11.1 
* Arm GNU Toolchain 13.2.rel1 (Build arm-13.7) 13.2.1 20231009
* SEGGER J-Link Software V7.96k

## External libraries

* CMSIS_5 5.9.0
* cmsis_device_l4 v1.7.2
* FreeRTOS-Kernel V10.4.3
* stm32l4xx_hal_driver v1.13.4

## Build

### Generate ninja files
`cmake --preset <hw_version>_<config>`

### Build project
`cmake --build --preset <hw_version>_<config>`
