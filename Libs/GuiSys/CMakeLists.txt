file(GLOB SOURCES *.cpp ../TypeSys.cpp)

add_library(GuiSys ${SOURCES})

target_include_directories(GuiSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#target_link_libraries(GuiSys PRIVATE cfsLib)