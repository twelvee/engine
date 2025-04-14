file(GLOB SOURCES *.cpp)

add_library(Math3D ${SOURCES})

target_include_directories(Math3D
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)