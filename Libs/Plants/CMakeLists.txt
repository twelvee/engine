file(GLOB SOURCES *.cpp)

add_library(Plants ${SOURCES})

target_include_directories(Plants
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)