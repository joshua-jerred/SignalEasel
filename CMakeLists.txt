cmake_minimum_required(VERSION 3.14)
project(SignalEasel VERSION 1.0 DESCRIPTION "A WAV file utility library for modulating audio")

# Configuration -------------------
option(USE_PULSEAUDIO "Use PulseAudio for audio input/output" ON)
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF)
option(UNIT_TESTS "Enable unit tests" ON)
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

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/WavGen)
if (SSTV_ENABLED)
    add_subdirectory(${CMAKE_SOURCE_DIR}/lib/SSTV-Image-Tools)
endif()

# --------------------------------
# SignalEasel
# --------------------------------
add_library(SignalEasel STATIC
    src/utilities.cpp
    src/bit_stream.cpp
    src/band_pass_filter.cpp
    src/ax25/ax25_address.cpp
    src/ax25/ax25_frame.cpp
    src/ax25/ax25_decode.cpp
    src/ax25/ax25_fcs.cpp
    src/modulator.cpp
    src/demodulator.cpp
    src/morse_modulator.cpp
    src/afsk_modulator.cpp
    src/afsk_demodulator.cpp
    src/aprs.cpp
    src/pulse_audio_writer.cpp
    src/pulse_audio_reader.cpp
    # ${MWAV_SRC}/modulators/psk.cpp
    # ${MWAV_SRC}/demodulators/psk.cpp
)
target_link_libraries(SignalEasel WavGen)
target_include_directories(SignalEasel 
    PUBLIC include 
    PRIVATE src
)
set_target_properties(SignalEasel PROPERTIES FOLDER include/signal_easel.hpp)

if(SSTV_ENABLED)
    message(STATUS "SSTV : Enabled")
    target_link_libraries(SignalEasel sstv-image-tools)
    add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=8)
    add_definitions(-DMAGICKCORE_HDRI_ENABLE=1) # Required or there are linking errors with Magick::Color::Color
    find_package(ImageMagick COMPONENTS Magick++)
    include_directories(${ImageMagick_INCLUDE_DIRS})
    target_link_libraries(SignalEasel ${ImageMagick_LIBRARIES})
else()
    message(STATUS "SSTV : Disabled")
endif()
unset(SSTV_ENABLED CACHE)

if(USE_PULSEAUDIO)
    message(STATUS "PulseAudio : Enabled")
    find_path(PULSEAUDIO_INCLUDE_DIR
        NAMES pulse/pulseaudio.h
    )
    find_library(PULSEAUDIO_LIBRARY
        NAMES pulse
    )
    include_directories(${PULSEAUDIO_INCLUDE_DIRS})
    target_link_libraries(SignalEasel pulse-simple pulse)
    target_compile_definitions(SignalEasel PRIVATE PULSE_AUDIO_ENABLED)
else()
    message(STATUS "PulseAudio : Disabled")
endif()

# --------------------------------
if (UNIT_TESTS)
    add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()

# add_executable(audio-in-test
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/demodulators/audio-in.cpp
#)

#add_executable(aprs-test
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/aprs.cpp
#)

