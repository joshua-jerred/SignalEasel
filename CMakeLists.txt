cmake_minimum_required(VERSION 3.25)
project(SignalEasel 
    VERSION 0.1
    DESCRIPTION "SignalEasel is a C++ library for audio modulation and demodulation through a variety of analog and digital modes."
    HOMEPAGE_URL "https://signaleasel.joshuajer.red"
)

# --- Configuration ---------------
# The following options can be set to enable/disable features.

option(USE_PULSEAUDIO "Use PulseAudio for audio input/output" ON)
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF)
option(SIGNALEASEL_UNIT_TESTS "Enable unit tests" ON)
option(SIGNALEASEL_COVERAGE "Enable code coverage" OFF)

# ---------------------------------

# # Check if SignalEasel is being used directly or via add_subdirectory
set(SIG_EAS_MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(SIG_EAS_MAIN_PROJECT ON)
endif()

message(STATUS "=== SignalEasel Main project: ${SIG_EAS_MAIN_PROJECT}")

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
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/WavGen EXCLUDE_FROM_ALL SYSTEM)

if(SSTV_ENABLED)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/SSTV-Image-Tools EXCLUDE_FROM_ALL SYSTEM)
endif()

if(SIG_EAS_MAIN_PROJECT)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/Boost_erSeat EXCLUDE_FROM_ALL SYSTEM)

    if(SIGNALEASEL_COVERAGE)
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
        list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/project/cmake)
        include(CodeCoverage)
    endif()
endif()

# --------------------------------
# SignalEasel
# --------------------------------
set(SignalEasel_sources
    src/utilities.cpp
    src/bit_stream.cpp
    src/band_pass_filter.cpp
    src/modulator.cpp
    src/demodulator.cpp
    src/morse_modulator.cpp

    # AFSK
    src/afsk/afsk_modulator.cpp
    src/afsk/afsk_demodulator.cpp
    src/afsk/afsk_receiver.cpp

    # AX.25
    src/ax25/ax25_address.cpp
    src/ax25/ax25_frame.cpp
    src/ax25/ax25_decode.cpp
    src/ax25/ax25_fcs.cpp

    # APRS
    src/aprs.cpp
    src/aprs/aprs_demodulator.cpp
    src/aprs/aprs_modulator.cpp
    src/aprs/aprs_receiver.cpp
    src/aprs/aprs_encoders.cpp
    src/aprs/telemetry_parameter.cpp
    src/aprs/telemetry_data.cpp
    src/aprs/telemetry_transcoder.cpp

    # PSK
    src/psk/psk_modulator.cpp
    src/psk/convolutional_code.cpp
    src/psk/varicode.cpp

    # PulseAudio
    src/pulse_audio_writer.cpp
    src/pulse_audio_reader.cpp
)
if(SSTV_ENABLED)
    list(APPEND SignalEasel_sources
        src/sstv/sstv_modulator.cpp
    )
endif()

add_library(SignalEasel STATIC
    ${SignalEasel_sources}
)
target_link_libraries(SignalEasel PRIVATE
    WavGen
    BoosterSeat
)
target_include_directories(SignalEasel
    PUBLIC include
    PRIVATE src
)
set_target_properties(SignalEasel PROPERTIES FOLDER include/signal_easel.hpp)

if(SSTV_ENABLED)
    message(STATUS "=== - SSTV          : ON")
    target_link_libraries(SignalEasel sstv-image-tools)
    add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=8)
    add_definitions(-DMAGICKCORE_HDRI_ENABLE=1) # Required or there are linking errors with Magick::Color::Color
    find_package(ImageMagick COMPONENTS Magick++)
    include_directories(${ImageMagick_INCLUDE_DIRS})
    target_link_libraries(SignalEasel PRIVATE ${ImageMagick_LIBRARIES})
    set(SSTV_TEST_ENABLED ON)
else()
    message(STATUS "=== - SSTV          : Disabled")
    set(SSTV_TEST_ENABLED OFF)
endif()

unset(SSTV_ENABLED CACHE)

if(USE_PULSEAUDIO)
    message(STATUS "=== - PulseAudio    : ON")
    find_path(PULSEAUDIO_INCLUDE_DIR
        NAMES pulse/pulseaudio.h
    )
    find_library(PULSEAUDIO_LIBRARY
        NAMES pulse
    )
    include_directories(${PULSEAUDIO_INCLUDE_DIRS})
    target_link_libraries(SignalEasel PRIVATE pulse-simple pulse)
    target_compile_definitions(SignalEasel PRIVATE PULSE_AUDIO_ENABLED)
else()
    message(STATUS "=== - PulseAudio    : Disabled")
endif()

# --------------------------------
# Code Coverage & Unit Tests
if(SIGNALEASEL_COVERAGE)
    message(STATUS "=== - Code Coverage : ON")
    append_coverage_compiler_flags_to_target(SignalEasel)
else()
    message(STATUS "=== - Code Coverage : Disabled")
endif()

if(SIGNALEASEL_UNIT_TESTS)
    message(STATUS "=== - Unit Tests    : ON")
    enable_testing()
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
else()
    message(STATUS "=== - Unit Tests    : Disabled")
endif()