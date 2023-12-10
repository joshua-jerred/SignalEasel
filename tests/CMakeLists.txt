# enable_testing()
find_package(GTest REQUIRED)

add_executable(signal_easel_unit_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/afsk_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/aprs_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ax25_crc_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ax25_frame_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ax25_address_test.cpp
)
target_link_libraries(signal_easel_unit_tests GTest::GTest GTest::Main SignalEasel)
target_include_directories(signal_easel_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/afsk_timing_skew.wav ${CMAKE_CURRENT_BINARY_DIR}/afsk_timing_skew.wav COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/white_noise.wav ${CMAKE_CURRENT_BINARY_DIR}/white_noise.wav COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/aprs_message.wav ${CMAKE_CURRENT_BINARY_DIR}/aprs_message.wav COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/aprs_real.wav ${CMAKE_CURRENT_BINARY_DIR}/aprs_real.wav COPYONLY)