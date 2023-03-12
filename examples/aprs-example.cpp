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
  std::cout << "\033[1;31m" << filename << "\033[0m\n";
  std::string response = exec("atest " + filename);

  std::size_t pos = response.find(source_call);
  if (pos == std::string::npos) {
    std::cout << "No decoded packet found." << std::endl;
    return;
  }
  response.erase(0, pos + source_call.length());
  response.erase(0, response.find(source_call));
  response = response.substr(0, response.find("\n"));

  response = exec("echo '" + response + "' | decode_aprs");
  response.erase(0, response.find(source_call));  // Remove the color
  std::cout << response << std::endl;
}
#else
void test(std::string filename, std::string source_call) {
  (void)source_call;
  std::cout << "File saved as: " << filename << std::endl;
}
#endif

#include "mwav.h"

int main() {
  mwav::AprsRequiredFields required_fields;
  required_fields.source_address = "TSTCLL";
  required_fields.source_ssid = 11;
  required_fields.symbol_table = mwav::AprsSymbolTable::PRIMARY;
  required_fields.symbol = 'O';

  // Location Data Packet Example
  mwav::aprs_packet::CompressedPositionReport location_data;
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

  // Telemetry
  mwav::aprs_packet::Telemetry telem;
  telem.telem_destination_address = "TSTCLL-11";
  telem.sequence_number = "001";
  telem.comment = "Testing Telemetry";
  telem.project_title = "Test project title.";

  telem.A1.name = "TEST1";
  telem.A1.unit = "UNIT1";
  telem.A1.value = "123";
  telem.A1.coef_a = "456";
  telem.A1.coef_b = "789";
  telem.A1.coef_c = "012";

  telem.A2.name = "TEST2";
  telem.A2.unit = "UNIT2";
  telem.A2.value = "789";

  telem.A3.name = "TEST3";
  telem.A3.unit = "UNIT3";
  telem.A3.value = "123";
  telem.A3.coef_a = "-.1";

  telem.A4.name = "TEST4";
  telem.A4.unit = "UNIT4";
  telem.A4.value = "456";
  telem.A4.coef_a = "0.1";

  telem.A5.name = "TEST5";
  telem.A5.unit = "UNIT5";
  telem.A5.value = "789";
  telem.A5.coef_c = "100";

  telem.D1.name = "TEST6";
  telem.D1.unit = "UNIT6";
  telem.D1.value = true;

  telem.D2.name = "TEST7";
  telem.D2.unit = "U7";
  telem.D2.value = false;

  telem.D3.name = "TE8";
  telem.D3.unit = "U8";
  telem.D3.value = true;

  telem.D4.name = "TE9";
  telem.D4.unit = "U9";
  telem.D4.value = false;

  telem.D5.name = "TE10";
  telem.D5.unit = "U10";
  telem.D5.value = true;

  telem.D6.name = "ALT";
  telem.D6.unit = "U11";
  telem.D6.value = false;

  telem.D7.name = "BKN";
  telem.D7.unit = "NEG";
  telem.D7.value = true;

  telem.D8.name = "LST";
  telem.D8.unit = "OK";
  telem.D8.value = false;

  mwav::aprs_packet::TelemetryPacketType telem_type =
      mwav::aprs_packet::TelemetryPacketType::DATA_REPORT;

  filename = "aprs-telem-data-example.wav";
  mwav::EncodeAprs(filename, required_fields, telem, telem_type);
  test(filename, required_fields.source_address);

  telem_type = mwav::aprs_packet::TelemetryPacketType::PARAM_NAME;
  filename = "aprs-telem-param-name-example.wav";
  mwav::EncodeAprs(filename, required_fields, telem, telem_type);
  test(filename, required_fields.source_address);

  telem_type = mwav::aprs_packet::TelemetryPacketType::PARAM_UNIT;
  filename = "aprs-telem-param-unit-example.wav";
  mwav::EncodeAprs(filename, required_fields, telem, telem_type);
  test(filename, required_fields.source_address);

  telem_type = mwav::aprs_packet::TelemetryPacketType::PARAM_COEF;
  filename = "aprs-telem-param-coef-example.wav";
  mwav::EncodeAprs(filename, required_fields, telem, telem_type);
  test(filename, required_fields.source_address);

  telem_type = mwav::aprs_packet::TelemetryPacketType::BIT_SENSE_PROJ_NAME;
  filename = "aprs-telem-bit-sense-example.wav";
  mwav::EncodeAprs(filename, required_fields, telem, telem_type);
  test(filename, required_fields.source_address);

  // Message
  mwav::aprs_packet::Message message;
  message.addressee = "TSTCLL-11";
  message.message = "Hello World!";
  message.message_id = "1";

  filename = "aprs-message-example.wav";
  mwav::EncodeAprs(filename, required_fields, message);
  test(filename, required_fields.source_address);

  // Message Acknowledgement
  mwav::aprs_packet::MessageAck ack;
  ack.addressee = "TSTCLL-11";
  ack.message_id = "1";

  filename = "aprs-message-ack-example.wav";
  mwav::EncodeAprs(filename, required_fields, ack);
  test(filename, required_fields.source_address);

  // Message Rejection
  mwav::aprs_packet::MessageNack rej;
  rej.addressee = "TSTCLL-11";
  rej.message_id = "1";

  filename = "aprs-message-rej-example.wav";
  mwav::EncodeAprs(filename, required_fields, rej);
  test(filename, required_fields.source_address);

  return 0;
}