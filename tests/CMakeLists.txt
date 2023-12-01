# enable_testing()
find_package(GTest REQUIRED)

add_executable(mwav_unit_tests
  ${MWAV_TESTS}/afsk_ascii_test.cpp
)
target_link_libraries(mwav_unit_tests GTest::GTest GTest::Main MWavLib WavGen)
target_include_directories(mwav_unit_tests PRIVATE)