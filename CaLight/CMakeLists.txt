add_executable(CaLight
        CaLight.cpp
        CaLightWorld.cpp
        $<TARGET_OBJECTS:CommonObjects>
)

target_link_libraries(CaLight ${COMMON_LIBS})