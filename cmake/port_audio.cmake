cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)
if(NO_INTERNET)
    set(LOCAL_PORT_AUDIO_DIR ${CMAKE_SOURCE_DIR}/../PortAudio CACHE PATH "Path to the local PortAudio directory" )
    message(STATUS "Looking for a local copy of ReferenceSoftwarePhase1 in ${LOCAL_PORT_AUDIO_DIR}")
    FetchContent_Declare(PORT_AUDIO URL ${LOCAL_PORT_AUDIO_DIR})
else()
    FetchContent_Declare(PORT_AUDIO
        GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
      )
endif()


FetchContent_MakeAvailable(PORT_AUDIO)

# Make MPEG Reference software header files available
include_directories(${CMAKE_BINARY_DIR}/_deps/port_audio-src/include)

