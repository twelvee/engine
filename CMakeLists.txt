cmake_minimum_required(VERSION 3.15)
project(Cafu)

include(FetchContent)

FetchContent_Declare(
        wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG        v3.2.2
)

set(wxBUILD_SHARED OFF CACHE BOOL "Build shared libraries" FORCE)
set(wxBUILD_SAMPLES OFF CACHE BOOL "Build wxWidgets samples" FORCE)
set(wxBUILD_TESTS OFF CACHE BOOL "Build wxWidgets tests" FORCE)
set(wxBUILD_DEMOS OFF CACHE BOOL "Build wxWidgets demos" FORCE)
# set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/wxInstall" CACHE PATH "Install path for wxWidgets" FORCE)

FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG        VER-2-13-0
)

set(FT_DISABLE_ZLIB TRUE CACHE BOOL "Disable zlib in freetype" FORCE)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.8
)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Do not build GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Do not build GLFW tests")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Do not build GLFW docs")

# Also brings zlib libs
FetchContent_Declare(
        minizip
        GIT_REPOSITORY https://github.com/zlib-ng/minizip-ng.git
        GIT_TAG        4.0.9
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build minizip as static library" FORCE)

FetchContent_Declare(
        ogg
        GIT_REPOSITORY https://github.com/xiph/ogg.git
        GIT_TAG        v1.3.5
)

FetchContent_Declare(
        vorbis
        GIT_REPOSITORY https://github.com/xiph/vorbis.git
        GIT_TAG        v1.3.7
)

FetchContent_Declare(
        openal-soft
        GIT_REPOSITORY https://github.com/kcat/openal-soft.git
        GIT_TAG        1.24.3
)


set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build OpenAL examples" FORCE)
set(BUILD_TESTING  OFF CACHE BOOL "Don't build OpenAL tests" FORCE)
set(OPENAL_SOFT_UTILS OFF CACHE BOOL "Don't build OpenAL utilities" FORCE)

FetchContent_Declare(
        bullet
        GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
        GIT_TAG        3.25
)

set(BUILD_EXTRAS         OFF CACHE BOOL "Don't build Bullet extras" FORCE)
set(BUILD_UNIT_TESTS     OFF CACHE BOOL "Don't build Bullet unit tests" FORCE)
set(BUILD_BULLET2_DEMOS  OFF CACHE BOOL "Don't build Bullet2 demos" FORCE)

FetchContent_MakeAvailable(wxWidgets)
FetchContent_MakeAvailable(freetype)
FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(minizip)
FetchContent_MakeAvailable(ogg)
FetchContent_MakeAvailable(vorbis)
FetchContent_MakeAvailable(openal-soft)
FetchContent_MakeAvailable(bullet)

set(OGG_INCLUDE_DIR "${ogg_SOURCE_DIR}/include" CACHE PATH "Ogg include directory" FORCE)
set(OGG_LIBRARY "$<TARGET_FILE:ogg>" CACHE FILEPATH "Ogg library file" FORCE)

# platform specific settings
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_WINSOCK_DEPRECATED_NO_WARNINGS)
    set(CMAKE_CXX_STANDARD 17)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR /EHsc /W3 /WX")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Od /Z7 /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /O2 /Ob2 /Z7")

    set(PLATFORM_LIBS wsock32 gdi32 opengl32 user32 shell32 advapi32 comctl32 comdlg32 ole32 oleaut32 rpcrt4 winspool)
elseif(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror -Wno-char-subscripts")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wl,--strip-all")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")

    set(PLATFORM_LIBS GL X11 Xrandr Xinerama Xxf86vm Xcursor rt dl pthread cairo pangocairo-1.0)

    include_directories(/usr/include/freetype2)
endif()

# General include dirs
include_directories(
        ${CMAKE_SOURCE_DIR}/ExtLibs
        ${CMAKE_SOURCE_DIR}/ExtLibs/tclap/include
        ${CMAKE_SOURCE_DIR}/ExtLibs/stbi
        ${CMAKE_SOURCE_DIR}/ExtLibs/lwo
        ${CMAKE_SOURCE_DIR}/ExtLibs/lua
        ${CMAKE_SOURCE_DIR}/Libs
        ${ogg_SOURCE_DIR}/include
        ${openal-soft_SOURCE_DIR}/include
        ${glfw_SOURCE_DIR}/include
)

# Common libraries
set(COMMON_LIBS
        BulletDynamics
        BulletCollision
        LinearMath
        TextParser
        Math3D
        Terrain
        Bitmap
        SceneGraph
        MatSys
        SoundSys
        GuiSys
        GameSys
        ClipSys
        Models
        Plants
        cfsLib
        ogg
        ${PLATFORM_LIBS}
)


add_subdirectory(Common)
add_subdirectory(CaBSP)
add_subdirectory(CaPVS)
add_subdirectory(CaLight)
add_subdirectory(CaSHL)
add_subdirectory(CaTools)
add_subdirectory(Ca3DE)
add_subdirectory(CaWE)

add_subdirectory(ExtLibs)
add_subdirectory(Libs)

if(EXISTS "${CMAKE_SOURCE_DIR}/ExtLibs/fbx/lib")
    add_definitions(-DHAVE_FBX_SDK)
endif()

# todo: remove
add_compile_definitions(SCONS_BUILD_DIR=build)