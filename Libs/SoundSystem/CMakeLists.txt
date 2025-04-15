file(GLOB SOURCES *.cpp Common/SoundStream.cpp Common/OggVorbisStream.cpp SoundSysNull/*.cpp SoundSysOpenAL/*.cpp)

add_library(SoundSys ${SOURCES})

target_include_directories(SoundSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${vorbis_SOURCE_DIR}/include
        ${openal-soft_SOURCE_DIR}/include
)

### DLLS
# Null
file(GLOB SOURCE_NullSystem SoundSysNull/*.cpp)

add_library(NullSoundSystem SHARED ${SOURCE_NullSystem})

# OpenAL
file(GLOB SOURCE_OpenALSystem Common/SoundStream.cpp Common/OggVorbisStream.cpp SoundSysOpenAL/*.cpp)

add_library(SoundSystemOA SHARED ${SOURCE_OpenALSystem})

target_link_libraries(SoundSystemOA ogg vorbis vorbisfile OpenAL)