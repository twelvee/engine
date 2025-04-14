file(GLOB SOURCES *.cpp)

add_library(TextParser ${SOURCES})

target_include_directories(TextParser
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)