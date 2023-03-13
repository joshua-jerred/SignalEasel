cmake_minimum_required(VERSION 3.14)
project(MWAV VERSION 1.0 DESCRIPTION "A WAV file utility library for modulating audio")
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -pedantic -Wall -Wextra -Weffc++ -Wdisabled-optimization -Wfloat-equal -Wno-unused-variable")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/SSTV-Image-Tools)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/WavGen)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WavGen
)

add_library(MWAVlib
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mwav.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/bit-stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/ax25.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/psk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/afsk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/morse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/aprs.cpp
)
target_link_libraries(MWAVlib WavGenlib)

set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)

add_executable(commandline-demo
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/commandline-demo.cpp
)

add_executable(psk-example
${CMAKE_CURRENT_SOURCE_DIR}/examples/psk-example.cpp
)

add_executable(ax25-example
${CMAKE_CURRENT_SOURCE_DIR}/examples/ax25-example.cpp 
)

add_executable(aprs-example ${CMAKE_CURRENT_SOURCE_DIR}/examples/aprs-example.cpp)

target_link_libraries(commandline-demo MWAVlib)
target_link_libraries(psk-example MWAVlib)
target_link_libraries(ax25-example MWAVlib)
target_link_libraries(aprs-example MWAVlib)

#add_executable(audio-in-test
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/demodulators/audio-in.cpp
#)

#add_executable(aprs-test
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/aprs.cpp
#)

#find_path(PULSEAUDIO_INCLUDE_DIR
#    NAMES pulse/pulseaudio.h
#)
#find_library(PULSEAUDIO_LIBRARY
#    NAMES pulse
#)
#include_directories(${PULSEAUDIO_INCLUDE_DIRS})
#target_link_libraries(audio-in-test pulse-simple pulse)