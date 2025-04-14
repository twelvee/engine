file(GLOB SOURCES src/*.cpp src/model/*.cpp src/module/*.cpp)
add_library(noise ${SOURCES})

target_include_directories(noise
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)