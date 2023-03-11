/**
 * @file aprs-example.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief An example of how to use the MWAV library to encode an APRS packet.
 * @details Use's the MWAV library to encode an APRS packet and save it to a WAV
 * file. If you have direwolf installed, the packet can be decoded if the
 * TEST_ENABLED macro is set to true.
 * @date 2023-03-11
 * @copyright Copyright (c) 2023
 * @version 0.9
 */

#include <iostream>
#include <string>

/**
 * @brief Set to true if you want to test the output wav file with direwolf.
 * This is targetted to Linux systems. If on Windows, set this to false.
 */
#define TEST_ENABLED true

#if TEST_ENABLED
#include <array>
#include <memory>
#include <stdexcept>

std::string exec(std::string command) {
  std::string response;
  std::array<char, 1028> out_buff;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed");
  }
  while (fgets(out_buff.data(), out_buff.size(), pipe.get()) != nullptr) {
    response += out_buff.data();
  }
  return response;
}

void test(std::string filename, std::string source_call) {
  std::string response = exec("atest " + filename);

  std::size_t pos = response.find(source_call);
  if (pos == std::string::npos) {
    std::cout << "No decoded packet found." << std::endl;
    return;
  }
  response.erase(0, pos + source_call.length());
  response.erase(0, response.find(source_call));
  response = response.substr(0, response.find("\n"));

  std::cout << exec("echo '" + response + "' | decode_aprs") << std::endl;
}
#else
void test(std::string filename, std::string source_call) {
  (void) source_call;
  std::cout << "File saved as: " << filename << std::endl;
}
#endif

#include "mwav.h"

int main() {
  mwav::AprsRequiredFields required_fields;
  mwav::aprs_packet::AprsLocationData location_data;

  required_fields.source_address = "TSTCLL";
  required_fields.source_ssid = 11;
  required_fields.symbol_table = mwav::AprsSymbolTable::PRIMARY;
  required_fields.symbol = 'O';
  required_fields.location_data = true;

  // Location Data Packet Example
  location_data.time_code = "092345";
  location_data.latitude = 38.517510;
  location_data.longitude = -104.351732;
  location_data.altitude = 88132;
  location_data.speed = 36.2;
  location_data.course = 88;
  location_data.comment = " Testing the MWAV APRS Implementation";

  std::string filename = "aprs-location-example.wav";

  mwav::EncodeAprs(filename, required_fields, location_data);

  test(filename, required_fields.source_address);
  return 0;
}
