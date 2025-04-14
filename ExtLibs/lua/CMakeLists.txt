file(GLOB SOURCES *.c)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/onelua.c")
add_library(lua ${SOURCES})

target_include_directories(lua
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)