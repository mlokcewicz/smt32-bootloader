# main app target
aux_source_directory(. MAIN_APP_SRC)

add_executable(${CMAKE_PROJECT_NAME}.elf ${MAIN_APP_SRC})

target_include_directories(${CMAKE_PROJECT_NAME}.elf PRIVATE .)

target_link_libraries(${CMAKE_PROJECT_NAME}.elf 
    cmsis_device_l4 
    cmsis_5 
    freertos_kernel 
    stm32l4xx_hal_driver
    utils 
    platform
)

target_link_options(${CMAKE_PROJECT_NAME}.elf PRIVATE 
    -T${PROJECT_SOURCE_DIR}/hal/startup/main_linker_script_l476.ld
    -L${PROJECT_SOURCE_DIR}/hal/startup
)

# custom targets
add_custom_target(boot_size ALL DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${SIZE} -B ${CMAKE_PROJECT_NAME}.elf)
add_custom_target(boot_bin ALL DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin)

