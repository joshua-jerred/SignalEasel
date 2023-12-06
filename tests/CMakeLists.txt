# enable_testing()
find_package(GTest REQUIRED)

add_executable(signal_easel_unit_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/afsk_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/aprs_test.cpp
)
target_link_libraries(signal_easel_unit_tests GTest::GTest GTest::Main SignalEasel)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/afsk_timing_skew.wav ${CMAKE_CURRENT_BINARY_DIR}/afsk_timing_skew.wav COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/white_noise.wav ${CMAKE_CURRENT_BINARY_DIR}/white_noise.wav COPYONLY)