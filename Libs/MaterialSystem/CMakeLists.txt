file(GLOB SOURCES *.cpp Common/*.cpp RendererARBprogs/*.cpp RendererNull/*.cpp RendererOpenGL12/*.cpp)

add_library(MatSys ${SOURCES})

target_include_directories(MatSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)