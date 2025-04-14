# CaWE/CMakeLists.txt

file(GLOB SOURCES
        *.cpp
        FontWizard/*.cpp
        GuiEditor/*.cpp GuiEditor/Commands/*.cpp
        MapEditor/*.cpp MapEditor/Commands/*.cpp
        MaterialBrowser/*.cpp
        ModelEditor/*.cpp ModelEditor/Commands/*.cpp
        MapEditor/wxExt/*.cpp
        MapEditor/wxFB/*.cpp
)

if(WIN32)
    list(APPEND SOURCES CaWE.rc)
endif()

add_executable(CaWE
        ${SOURCES}
        $<TARGET_OBJECTS:CommonObjects>
)


target_link_libraries(CaWE
        PRIVATE
        ${COMMON_LIBS}
        wx::base
        wx::core
        wx::aui
        wx::gl
        wx::propgrid
        freetype
)

if(HAVE_FBX_SDK)
    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(FBX_LIB_PATH "${CMAKE_SOURCE_DIR}/ExtLibs/fbx/lib/vs2015/x64/release")
        else()
            set(FBX_LIB_PATH "${CMAKE_SOURCE_DIR}/ExtLibs/fbx/lib/vs2015/x86/release")
        endif()
        target_link_libraries(CaWE "${FBX_LIB_PATH}/libfbxsdk-md.lib")
    else()
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(FBX_LIB_PATH "${CMAKE_SOURCE_DIR}/ExtLibs/fbx/lib/gcc4/x64/release")
        else()
            set(FBX_LIB_PATH "${CMAKE_SOURCE_DIR}/ExtLibs/fbx/lib/gcc4/x86/release")
        endif()
        target_link_libraries(CaWE "${FBX_LIB_PATH}/libfbxsdk.a")
    endif()
endif()

if(UNIX AND NOT APPLE)
    target_link_options(CaWE PRIVATE "-Wl,-rpath,." "-Wl,--export-dynamic")
endif()

# Пример пользовательской команды
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    #[[add_custom_command(
            TARGET CaWE POST_BUILD
            COMMAND CaWE --update-doxygen
            COMMENT "Updating documentation templates"
    )]]
endif()
