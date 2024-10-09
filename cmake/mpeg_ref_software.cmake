cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)

if(NO_INTERNET)
    set(LOCAL_MPEG_REF_SOFTWARE_DIR ${CMAKE_SOURCE_DIR}/../ReferenceSoftwarePhase1 CACHE PATH "Path to the local ReferenceSoftwarePhase1 directory" )
    message(STATUS "Looking for a local copy of ReferenceSoftwarePhase1 in ${LOCAL_MPEG_REF_SOFTWARE_DIR}")
    FetchContent_Declare(MPEG_REF_SOFTWARE URL ${LOCAL_MPEG_REF_SOFTWARE_DIR})
else()
    FetchContent_Declare(MPEG_REF_SOFTWARE
        GIT_REPOSITORY https://git.mpeg.expert/MPEG/3dgh/haptics/software/ReferenceSoftwarePhase1.git
        GIT_TAG "e3a5efcb4f6284faa96788df595e6649be5b38cb"
	    CONFIGURE_COMMAND ""
	    BUILD_COMMAND "")
endif()

FetchContent_MakeAvailable(MPEG_REF_SOFTWARE)

# Make MPEG Reference software header files available
#include_directories(${CMAKE_BINARY_DIR}/_deps/mpeg_ref_software-src/source/Types/include)
include_directories(${CMAKE_BINARY_DIR}/_deps/mpeg_ref_software-src/source)
include_directories(${CMAKE_BINARY_DIR}/_deps/rapidjson-src/include)
#include_directories(${CMAKE_BINARY_DIR}/_deps/mpeg_ref_software-src/source/Synthesizer/include)
#include_directories(${CMAKE_BINARY_DIR}/_deps/mpeg_ref_software-src/source/Tools/include)