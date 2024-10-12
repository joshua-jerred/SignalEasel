include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  #${CMAKE_CURRENT_SOURCE_DIR}/../src/modulators
)
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
# add_executable(commandline-demo ${EXAMPLES_DIR}/commandline-demo.cpp) out of date
add_executable(psk-example ${MWAV_EXAMPLES}/psk-example.cpp)
add_executable(ax25-example ${MWAV_EXAMPLES}/ax25-example.cpp)
add_executable(aprs-example ${MWAV_EXAMPLES}/aprs-example.cpp)

# target_link_libraries(commandline-demo MWAVlib)
target_link_libraries(psk-example MWAVlib)
target_link_libraries(ax25-example MWAVlib)
target_link_libraries(aprs-example MWAVlib)

if (SSTV_ENABLED)
  add_executable(sstv-example ${MWAV_EXAMPLES}/sstv-example.cpp)
  add_executable(sstvgen ${MWAV_EXAMPLES}/sstv-commandline.cpp)

  target_link_libraries(sstv-example MWAVlib)
  target_link_libraries(sstvgen MWAVlib)

  target_link_libraries(sstv-example sstv-image-tools)
  target_link_libraries(sstvgen sstv-image-tools)
  configure_file(${EXAMPLES_DIR}/test1.png ${CMAKE_CURRENT_BINARY_DIR}/test1.png COPYONLY)
endif()