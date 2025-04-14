# Tools have slightly different library requirements
set(TOOLS_LIBS ${COMMON_LIBS} freetype)

add_executable(MakeFont MakeFont.cpp)
target_link_libraries(MakeFont ${TOOLS_LIBS})

add_executable(CaSanity CaSanity.cpp $<TARGET_OBJECTS:CommonObjects>)
target_link_libraries(CaSanity ${TOOLS_LIBS})

add_executable(MaterialViewer MaterialViewer.cpp)
target_link_libraries(MaterialViewer ${TOOLS_LIBS})

add_executable(TerrainViewer TerrainViewer.cpp)
target_link_libraries(TerrainViewer ${TOOLS_LIBS})