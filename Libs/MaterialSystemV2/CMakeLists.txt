file(GLOB SOURCES *.cpp)

add_library(MatSysV2 ${SOURCES})

target_include_directories(MatSysV2
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}
)

# OPENGL 4.6
file(GLOB SOURCE_OGL46 OpenGL/*.cpp OpenGL/Shaders/A_Solid.cpp)

add_library(RendererOpenGL46 SHARED ${SOURCE_OGL46})
add_definitions(-DDEBUG=true)

target_link_libraries(RendererOpenGL46 ${COMMON_LIBS} libglew_static)