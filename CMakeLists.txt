cmake_minimum_required(VERSION 3.14)
project(MWAV VERSION 1.0 DESCRIPTION "A WAV file utility library for modulating audio")

# Configuration -------------------
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF)
option(MWAV_UNIT_TESTS "Enable unit tests" ON)
# ---------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-fno-omit-frame-pointer \
	-pedantic \
	-Wall \
	-Wextra \
	-Weffc++ \
	-Wunused-variable \
	-Wno-type-limits \
	-Wno-float-equal")
# no type limits and no float equal are for the ImageMagick library

set(MWAV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MWAV_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MWAV_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(MWAV_EXAMPLES ${CMAKE_CURRENT_SOURCE_DIR}/examples)
set(MWAV_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_subdirectory(${MWAV_LIB}/WavGen)
if (SSTV_ENABLED)
    add_subdirectory(${MWAV_LIB}/SSTV-Image-Tools)
endif()

# --------------------------------
# MWavLib
# --------------------------------
add_library(MWavLib
    ${MWAV_SRC}/data_modulation.cpp
    ${MWAV_SRC}/modulators/afsk.cpp
    ${MWAV_SRC}/modulators/psk.cpp
    ${MWAV_SRC}/utilities/bit-stream.cpp
)
target_link_libraries(MWavLib WavGen)
target_include_directories(MWavLib 
    PUBLIC ${MWAV_INCLUDE}
    PRIVATE ${MWAV_SRC}
    ${MWAV_SRC}/modulators
    ${MWAV_SRC}/demodulators
    ${MWAV_SRC}/utilities
)


if(SSTV_ENABLED)
    message(STATUS "SSTV Enabled")
    target_link_libraries(MWavLib sstv-image-tools)
    add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=8)
    add_definitions(-DMAGICKCORE_HDRI_ENABLE=1) # Required or there are linking errors with Magick::Color::Color
    find_package(ImageMagick COMPONENTS Magick++)
    include_directories(${ImageMagick_INCLUDE_DIRS})
    target_link_libraries(MWavLib ${ImageMagick_LIBRARIES})
else()
    message(STATUS "SSTV Disabled")
endif()
unset(SSTV_ENABLED CACHE)

# --------------------------------
if (MWAV_UNIT_TESTS)
    add_subdirectory(${MWAV_TESTS})
endif()

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