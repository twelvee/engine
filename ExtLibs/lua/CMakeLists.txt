file(GLOB SOURCES *.c)

add_library(lua ${SOURCES})

target_include_directories(lua
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)