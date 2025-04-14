file(GLOB SOURCES *.cpp Common/SoundStream.cpp Common/OggVorbisStream.cpp SoundSysNull/*.cpp SoundSysOpenAL/*.cpp)

add_library(SoundSys ${SOURCES})

target_include_directories(SoundSys
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${vorbis_SOURCE_DIR}/include
        ${openal-soft_SOURCE_DIR}/include
        ../../ExtLibs/freealut/include
)