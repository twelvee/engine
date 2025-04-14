file(GLOB SOURCES *.cpp HumanPlayer/*.cpp)

add_library(GameSys ${SOURCES})

target_include_directories(GameSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        "${bullet_SOURCE_DIR}/src"
)

#target_link_libraries(GameSys PRIVATE cfsLib)