cmake_minimum_required(VERSION 3.14)
project(MWAV VERSION 1.0 DESCRIPTION "A WAV file utility library for modulating audio")
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -pedantic -Wall -Wextra -Weffc++ -Wdisabled-optimization -Wfloat-equal -Wno-unused-variable")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators
)

add_executable(commandline
    ${CMAKE_CURRENT_SOURCE_DIR}/src/commandline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mwav.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/wavgen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/bit-stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/ax25.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/psk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/afsk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/morse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/aprs.cpp
)

add_executable(psk-example
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/psk-example.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mwav.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/psk.cpp
)

add_executable(ax25-example
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/ax25-example.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/bit-stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/wavgen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/ax25.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/afsk.cpp
)

add_executable(aprs-example
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/aprs-example.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/bit-stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/wavgen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/ax25.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/afsk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/aprs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mwav.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/psk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/morse.cpp
)

#add_executable(aprs-testing
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/aprs-testing.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/mwav.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/wavgen.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/bit-stream.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/ax25.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/psk.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/afsk.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/morse.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/aprs.cpp
#)

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