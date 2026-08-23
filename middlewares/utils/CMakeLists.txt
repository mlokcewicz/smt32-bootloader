# utils target
add_library(utils STATIC)

target_include_directories(utils PUBLIC .)

target_sources(utils PRIVATE data_exchange.c)