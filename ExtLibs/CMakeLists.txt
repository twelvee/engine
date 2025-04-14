# --- fbx ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fbx/CMakeLists.txt")
    add_subdirectory(fbx)
endif()

# --- fmod ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fmod/CMakeLists.txt")
    add_subdirectory(fmod)
endif()

# --- lwo ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lwo/CMakeLists.txt")
    add_subdirectory(lwo)
endif()

# --- lua ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lua/CMakeLists.txt")
    add_subdirectory(lua)
endif()

# --- noise ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/noise/CMakeLists.txt")
    add_subdirectory(noise)
endif()

# --- tclap ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tclap/CMakeLists.txt")
    add_subdirectory(tclap)
endif()