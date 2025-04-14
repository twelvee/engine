add_executable(CaBSP
        CaBSP.cpp
        BspTreeBuilder/BspTreeBuilder.cpp
        $<TARGET_OBJECTS:CommonObjects>
)

target_link_libraries(CaBSP ${COMMON_LIBS})