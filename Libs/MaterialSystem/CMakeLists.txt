file(GLOB SOURCES *.cpp Common/*.cpp RendererARBprogs/*.cpp RendererNull/*.cpp RendererOpenGL12/*.cpp)

add_library(MatSys ${SOURCES})

target_include_directories(MatSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB SOURCE_OGL RendererOpenGL12/*.cpp)

add_library(RendererOpenGL12 SHARED ${SOURCE_OGL})

target_link_libraries(RendererOpenGL12 ${COMMON_LIBS} glu32)