# external targets

# cmsis_5 target
add_library(cmsis_5 INTERFACE)

target_include_directories(cmsis_5 INTERFACE CMSIS_5/CMSIS/Core/Include)

# cmsis_device_l4_target
add_library(cmsis_device_l4 STATIC)

target_compile_definitions(cmsis_device_l4 PUBLIC -DSTM32L476xx)

target_include_directories(cmsis_device_l4 PUBLIC cmsis_device_l4/Include)

aux_source_directory(cmsis_device_l4/Source/Templates CMSIS_DEVICE_L4_SRC)
target_sources(cmsis_device_l4 PRIVATE ${CMSIS_DEVICE_L4_SRC})

target_link_libraries(cmsis_device_l4 cmsis_5)

# freertos_kernel target
add_library(freertos_kernel STATIC)

target_include_directories(freertos_kernel PUBLIC FreeRTOS-Kernel/include)
target_include_directories(freertos_kernel PUBLIC FreeRTOS-Kernel/portable/GCC/ARM_CM4F)

aux_source_directory(FreeRTOS-Kernel FREERTOS_KERNEL_SRC)
aux_source_directory(FreeRTOS-Kernel/portable/GCC/ARM_CM4F FREERTOS_KERNEL_SRC)
list(APPEND FREERTOS_KERNEL_SRC FreeRTOS-Kernel/portable/MemMang/heap_4.c)
target_sources(freertos_kernel PRIVATE ${FREERTOS_KERNEL_SRC})

set_target_properties(freertos_kernel PROPERTIES EXCLUDE_FROM_ALL True)

