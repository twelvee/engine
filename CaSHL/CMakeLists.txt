add_executable(CaSHL
        CaSHL.cpp
        CaSHLWorld.cpp
        $<TARGET_OBJECTS:CommonObjects>
)

target_link_libraries(CaSHL ${COMMON_LIBS})