cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)
if(NO_INTERNET)
    set(LOCAL_AUDIO_FILE_DIR ${CMAKE_SOURCE_DIR}/../AudioFile CACHE PATH "Path to the local AudioFile directory" )
    message(STATUS "Looking for a local copy of ReferenceSoftwarePhase1 in ${LOCAL_AUDIO_FILE_DIR}")
    FetchContent_Declare(AUDIO_FILE URL ${LOCAL_AUDIO_FILE_DIR})
else()
    FetchContent_Declare(AUDIO_FILE
        GIT_REPOSITORY https://github.com/adamstark/AudioFile.git
        GIT_TAG "develop"
	CONFIGURE_COMMAND ""
	BUILD_COMMAND "")
endif()


FetchContent_MakeAvailable(AUDIO_FILE)

# Make MPEG Reference software header files available
include_directories(${CMAKE_BINARY_DIR}/_deps/audio_file-src)

