# include(CTest)
find_package(GTest REQUIRED)
include(GoogleTest)

# Do this before defining the test executable so that we can add to it.
set(signal_easel_unit_tests_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/afsk_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/aprs_telemetry_data_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/aprs_telemetry_parameter_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/aprs_telemetry_transcoder_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/aprs_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ax25_address_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ax25_crc_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ax25_frame_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/psk_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utilities_test.cpp
)

if(SSTV_TEST_ENABLED)
  message(STATUS "Including SSTV tests")
  list(APPEND signal_easel_unit_tests_sources ${CMAKE_CURRENT_SOURCE_DIR}/sstv_test.cpp)
endif()

add_executable(signal_easel_unit_tests
  ${signal_easel_unit_tests_sources}
)
target_link_libraries(signal_easel_unit_tests GTest::GTest GTest::Main SignalEasel BoosterSeat)
# target_link_libraries(signal_easel_unit_tests SignalEasel)
target_include_directories(signal_easel_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
gtest_discover_tests(signal_easel_unit_tests)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/afsk_timing_skew.wav ${CMAKE_CURRENT_BINARY_DIR}/afsk_timing_skew.wav COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/white_noise.wav ${CMAKE_CURRENT_BINARY_DIR}/white_noise.wav COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/aprs_message.wav ${CMAKE_CURRENT_BINARY_DIR}/aprs_message.wav COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/aprs_real.wav ${CMAKE_CURRENT_BINARY_DIR}/aprs_real.wav COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_image.png ${CMAKE_CURRENT_BINARY_DIR}/test_image.png COPYONLY)

add_executable(pulse_test ${CMAKE_CURRENT_SOURCE_DIR}/pulse_test.cpp)
target_link_libraries(pulse_test SignalEasel BoosterSeat)
