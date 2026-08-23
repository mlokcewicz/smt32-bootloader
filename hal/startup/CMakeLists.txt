# startup target
add_library(startup STATIC)

target_include_directories(startup PUBLIC .)

target_sources(startup PUBLIC
    syscalls_l476.c
    vectors_l476.c
)
