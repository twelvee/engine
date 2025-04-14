file(GLOB SOURCES
        *.cpp
        Client/*.cpp
        Server/*.cpp
)

if(WIN32)
    list(APPEND SOURCES Cafu.rc)
endif()

add_executable(Cafu ${SOURCES} $<TARGET_OBJECTS:CommonObjects>)
target_link_libraries(Cafu ${COMMON_LIBS})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Cafu PRIVATE DEBUG=1)
else()
    target_compile_definitions(Cafu PRIVATE DEBUG=0)
endif()

if(UNIX AND NOT APPLE)
    target_link_options(Cafu PRIVATE "-Wl,-rpath,." "-Wl,--export-dynamic")
endif()