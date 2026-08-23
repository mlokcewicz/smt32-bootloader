# main app target
aux_source_directory(. BOOT_APP_SRC)

add_executable(boot_${CMAKE_PROJECT_NAME}.elf ${BOOT_APP_SRC})

target_include_directories(boot_${CMAKE_PROJECT_NAME}.elf PRIVATE .)

target_link_libraries(boot_${CMAKE_PROJECT_NAME}.elf 
    cmsis_device_l4 
    cmsis_5 
    freertos_kernel 
    stm32l4xx_hal_driver
    utils 
    platform
)

target_sources(boot_${CMAKE_PROJECT_NAME}.elf PRIVATE
    ${PROJECT_SOURCE_DIR}/hal/startup/startup_l476_ram.c
)

target_link_options(boot_${CMAKE_PROJECT_NAME}.elf PRIVATE 
    -T${PROJECT_SOURCE_DIR}/hal/startup/boot_linker_script_l476.ld
    -L${PROJECT_SOURCE_DIR}/hal/startup
)

# custom targets
add_custom_target(size ALL DEPENDS boot_${CMAKE_PROJECT_NAME}.elf COMMAND ${SIZE} -B boot_${CMAKE_PROJECT_NAME}.elf)
add_custom_target(bin ALL DEPENDS boot_${CMAKE_PROJECT_NAME}.elf COMMAND ${OBJCOPY} -O binary boot_${CMAKE_PROJECT_NAME}.elf boot_${CMAKE_PROJECT_NAME}.bin)
