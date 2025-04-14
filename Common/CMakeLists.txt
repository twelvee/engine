# Common source files used by multiple targets
add_library(CommonObjects OBJECT
        World.cpp
        CompGameEntity.cpp
        WorldMan.cpp
)

# Make the object files available to other directories
target_include_directories(CommonObjects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})