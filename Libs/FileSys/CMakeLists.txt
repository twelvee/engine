file(GLOB SOURCES *.cpp)

add_library(FileSys ${SOURCES})

target_include_directories(FileSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(FileSys PRIVATE minizip)