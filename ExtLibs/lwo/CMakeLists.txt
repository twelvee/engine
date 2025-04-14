file(GLOB SOURCES *.c)

add_library(lwo ${SOURCES})

target_include_directories(lwo
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)