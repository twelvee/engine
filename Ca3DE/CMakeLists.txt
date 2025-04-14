file(GLOB SOURCES
        *.cpp
        Client/*.cpp
        Server/*.cpp
)

if(WIN32)
    list(APPEND SOURCES Cafu.rc)
endif()

add_executable(Cafu ${SOURCES} $<TARGET_OBJECTS:CommonObjects>)
target_link_libraries(Cafu ${COMMON_LIBS} glfw)

if(UNIX AND NOT APPLE)
    target_link_options(Cafu PRIVATE "-Wl,-rpath,." "-Wl,--export-dynamic")
endif()