cmake_minimum_required(VERSION 3.14)
project(MWAV VERSION 1.0 DESCRIPTION "A WAV file utility library for modulating audio")
set(CMAKE_CXX_STANDARD 20)
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

set(CMAKE_BUILD_TYPE Debug) # Change to Release for production

# Configuration
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" ON) # ON or OFF
# --------------------------------

if(SSTV_ENABLED)
    message(STATUS "SSTV Enabled")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/SSTV-Image-Tools)
    add_compile_definitions(SSTV_ENABLED=1)
else()
    message(STATUS "SSTV Disabled")
    add_compile_definitions(SSTV_ENABLED=0)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/WavGen)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulators
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WavGen
)
if (SSTV_ENABLED)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SSTV-Image-Tools/include
    )
endif()

# --------------------------------
# MWAVlib
# --------------------------------
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MWAVlib_Utility_Sources
    ${SRC}/utilities/bit-stream.cpp
)
set(MWAVlib_Modulator_Sources
    ${SRC}/modulators/ax25.cpp
    ${SRC}/modulators/psk.cpp
    ${SRC}/modulators/afsk.cpp
    ${SRC}/modulators/morse.cpp
    ${SRC}/modulators/aprs.cpp
)
if (SSTV_ENABLED)
    set(MWAVlib_Modulator_Sources
        ${MWAVlib_Modulator_Sources}
        ${SRC}/modulators/sstv.cpp
    )
endif()

add_library(MWAVlib
    ${SRC}/mwav.cpp
    ${MWAVlib_Utility_Sources}
    ${MWAVlib_Modulator_Sources}
)

target_link_libraries(MWAVlib WavGenlib)
if(SSTV_ENABLED)
    # Eventually this could just be statically linked
    target_link_libraries(MWAVlib sstv-image-tools)
    add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=8)
    add_definitions(-DMAGICKCORE_HDRI_ENABLE=1) # Required or there are linking errors with Magick::Color::Color
    find_package(ImageMagick COMPONENTS Magick++)
    include_directories(${ImageMagick_INCLUDE_DIRS})
    target_link_libraries(MWAVlib ${ImageMagick_LIBRARIES})
endif()


# --------------------------------
# Examples
set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)
add_subdirectory(examples EXCLUDE_FROM_ALL)
# --------------------------------


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