file(GLOB SOURCES *.cpp Common/*.cpp)

add_library(MatSys ${SOURCES})

target_include_directories(MatSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


### DLLS
# OPENGL 1.2
file(GLOB SOURCE_OGL RendererOpenGL12/*.cpp)

add_library(RendererOpenGL12 SHARED ${SOURCE_OGL})

target_link_libraries(RendererOpenGL12 ${COMMON_LIBS} glu32)

# OPENGL 4.6
file(GLOB SOURCE_OGL46 Common/*.cpp RendererOpenGL46/*.cpp RendererOpenGL46/Shaders/A_Solid.cpp)

add_library(RendererOpenGL46 SHARED ${SOURCE_OGL46})
add_definitions(-DDEBUG=true)

target_link_libraries(RendererOpenGL46 ${COMMON_LIBS} libglew_static)