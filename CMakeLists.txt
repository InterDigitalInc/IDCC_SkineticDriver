cmake_minimum_required(VERSION 3.13)
project(Skinetic_Driver)

set(CMAKE_CXX_STANDARD 17)

option(NO_INTERNET "Use pre-downloaded source archives for external libraries, e.g. Port audio" OFF)

include(cmake/mpeg_ref_software.cmake)
include(cmake/audio_file.cmake)
include(cmake/port_audio.cmake)

#add_subdirectory(${CMAKE_BINARY_DIR}/_deps/mpeg_ref_software-src/source)

# Add the source directory to include the main.cpp file(s)
#set(LIBRARIES_DIR out/build/x64-Debug/_deps)
add_library(Skinetic_Driver SHARED ${CMAKE_SOURCE_DIR}/source/MPEG_Haptic_SKINETICDRIVER.cpp)
#add_executable(Skinetic_Driver ${CMAKE_SOURCE_DIR}/source/MPEG_Haptic_SKINETICDRIVER.cpp)

target_include_directories(Skinetic_Driver PRIVATE ${CMAKE_SOURCE_DIR}/source)

# Set the output directory for DLLs
set_target_properties(Skinetic_Driver PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
# Link the external library (assuming extlib defines a CMake target named extlib)
target_link_libraries(Skinetic_Driver PRIVATE  iohaptics types SynthesizerLib PortAudio)