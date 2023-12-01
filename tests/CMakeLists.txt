# enable_testing()
find_package(GTest REQUIRED)

add_executable(mwav_unit_tests
  ${MWAV_TESTS}/afsk_ascii_test.cpp
)
target_link_libraries(mwav_unit_tests GTest::GTest GTest::Main MWavLib WavGen)
target_include_directories(mwav_unit_tests PRIVATE)

configure_file(${MWAV_TESTS}/afsk_timing_skew.wav ${CMAKE_CURRENT_BINARY_DIR}/afsk_timing_skew.wav COPYONLY)
configure_file(${MWAV_TESTS}/white_noise.wav ${CMAKE_CURRENT_BINARY_DIR}/white_noise.wav COPYONLY)