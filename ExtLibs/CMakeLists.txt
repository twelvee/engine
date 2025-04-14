# --- fbx ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fbx/CMakeLists.txt")
    add_subdirectory(fbx)
endif()

# --- fmod ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fmod/CMakeLists.txt")
    add_subdirectory(fmod)
endif()

# --- freetype ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/freetype/CMakeLists.txt")
    add_subdirectory(freetype)
endif()

# --- libogg ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libogg/CMakeLists.txt")
    add_subdirectory(libogg)
endif()

# --- libvorbis ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/CMakeLists.txt")
    add_subdirectory(libvorbis)
endif()

# --- lua ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lua/CMakeLists.txt")
    add_subdirectory(lua)
endif()

# --- lwo ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lwo/CMakeLists.txt")
    add_subdirectory(lwo)
endif()

# --- mpg123 ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/mpg123/CMakeLists.txt")
    add_subdirectory(mpg123)
endif()

# --- noise ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/noise/CMakeLists.txt")
    add_subdirectory(noise)
endif()

# --- tclap ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tclap/CMakeLists.txt")
    add_subdirectory(tclap)
endif()