cmake_minimum_required(VERSION 3.14)
project(MWAV VERSION 0.9 DESCRIPTION "A WAV file utility library for modulating audio")
set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators
)

add_executable(commandline
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mwav.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/wavgen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/bit-stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/psk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/afsk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators/morse.cpp
)