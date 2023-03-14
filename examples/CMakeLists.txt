include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  #${CMAKE_CURRENT_SOURCE_DIR}/../src/modulators
)
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(commandline-demo ${EXAMPLES_DIR}/commandline-demo.cpp)
add_executable(psk-example ${EXAMPLES_DIR}/psk-example.cpp)
add_executable(ax25-example ${EXAMPLES_DIR}/ax25-example.cpp)
add_executable(aprs-example ${EXAMPLES_DIR}/aprs-example.cpp)

target_link_libraries(commandline-demo MWAVlib)
target_link_libraries(psk-example MWAVlib)
target_link_libraries(ax25-example MWAVlib)
target_link_libraries(aprs-example MWAVlib)

if (SSTV_ENABLED)
  add_executable(sstv-example ${EXAMPLES_DIR}/sstv-example.cpp)
  target_link_libraries(sstv-example MWAVlib)
  target_link_libraries(sstv-example sstv-image-tools)
  configure_file(${EXAMPLES_DIR}/test1.png ${CMAKE_CURRENT_BINARY_DIR}/test1.png COPYONLY)
endif()