cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

# toolchain configuration
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/hal/platforms/${HW_VERSION}/toolchain.cmake)

# project configuration
project("stm32_bootloader" C CXX ASM)

# Application modules
add_subdirectory(app)
add_subdirectory(boot)
add_subdirectory(middlewares)
add_subdirectory(external)
add_subdirectory(hal)

# global headers (FreeRTOSConfig.h, stm32l4xx_hal_conf.h)
target_include_directories(freertos_kernel PRIVATE hal/platforms/${HW_VERSION})
target_include_directories(stm32l4xx_hal_driver PRIVATE hal/platforms/${HW_VERSION})
