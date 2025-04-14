file(GLOB SOURCES *.cpp)

add_library(ClipSys ${SOURCES})

target_include_directories(ClipSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        "${bullet_SOURCE_DIR}/src"
)