file(GLOB SOURCES *.cpp)

add_library(Terrain ${SOURCES})

target_include_directories(Terrain
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)