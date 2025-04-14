file(GLOB SOURCES *.cpp)

add_library(Models ${SOURCES})

target_include_directories(Models
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Models PRIVATE lua lwo)