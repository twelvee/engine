add_executable(CaPVS
        CaPVS.cpp
        CaPVSWorld.cpp
        $<TARGET_OBJECTS:CommonObjects>
)

target_link_libraries(CaPVS ${COMMON_LIBS})