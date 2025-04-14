file(GLOB SOURCES *.cpp)

add_library(Bitmap ${SOURCES})

target_include_directories(Bitmap
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)