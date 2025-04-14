file(GLOB SOURCES *.cpp)

add_library(SceneGraph ${SOURCES})

target_include_directories(SceneGraph
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)