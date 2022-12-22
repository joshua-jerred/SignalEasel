cmake_minimum_required(VERSION 3.14)
project(MWAV VERSION 0.9 DESCRIPTION "A WAV file utility library for modulating audio")
set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_library(WavGen STATIC
    lib/wavgen.cpp
)

add_library(MWAV STATIC
    src/mwav.cpp
    src/modulators/psk.cpp
)

target_include_directories(MWAV PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators
    )

target_link_libraries(MWAV WavGen)

#
#add_executable(commandline
#    main.cpp
#)