cmake_minimum_required(VERSION 3.13...3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(pico_push VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE LIB_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/pico_push/src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/pico_push/src/*.c"
)

add_library(pico_push STATIC ${LIB_SOURCES})

target_include_directories(pico_push
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/pico_push/include"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/pico_push/src"
)

target_link_libraries(pico_push PUBLIC
    pico_stdlib
)

target_compile_definitions(pico_push PUBLIC
    _pico_push_MAJOR=${PICO_PUSH_VERSION_MAJOR}
    _pico_push_MINOR=${PICO_PUSH_VERSION_MINOR}
    _pico_push_PATCH=${PICO_PUSH_VERSION_PATCH}
)
