/**
 * @file aprs_demodulator.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of the APRS demodulator class.
 * @date 2024-01-14
 * @copyright Copyright (c) 2024
 */

#include <cmath>

#include <SignalEasel/aprs.hpp>

namespace signal_easel::aprs {

void Demodulator::printFrame() {
  std::cout << "Frame: " << frame_ << std::endl;
}

bool Demodulator::lookForAx25Packet() {
  ax25::Frame frame;
  if (!frame.parseBitStream(output_bit_stream_)) {
    type_ = aprs::Packet::Type::UNKNOWN;
    return false;
  }

  auto info = frame.getInformation();
  if (info.size() < 1) {
    type_ = aprs::Packet::Type::UNKNOWN;
    return false;
  }

  frame_ = frame;

  uint8_t first_byte = info.at(0);
  switch (first_byte) {
  case '@':
  case '/':
    type_ = aprs::Packet::Type::POSITION;
    break;
  case ':':
    type_ = aprs::Packet::Type::MESSAGE;
    break;

  default:
    type_ = aprs::Packet::Type::UNKNOWN;
    return false;
  }

  return true;
}

bool Demodulator::parseMessagePacket(aprs::MessagePacket &message_packet) {
  if (type_ != aprs::Packet::Type::MESSAGE) {
    return false;
  }

  auto info = frame_.getInformation();

  constexpr size_t k_min_message_length = 10;

  if (info.size() < k_min_message_length) {
    return false;
  }

  // ":<addressee (9)>:<message>{<id>"
  if (info.at(0) != ':' || info.at(10) != ':') {
    return false;
  }

  message_packet.addressee = std::string(info.begin() + 1, info.begin() + 10);

  std::string content(info.begin() + 11, info.end());
  size_t message_end = content.find('{');
  if (message_end == std::string::npos) {
    return false;
  }
  message_packet.message = content.substr(0, message_end);
  if (message_end + 1 < content.length()) {
    message_packet.message_id = content.substr(message_end + 1);
  }

  return true;
}

bool Demodulator::parsePositionPacket(aprs::PositionPacket &position) {
  if (type_ != aprs::Packet::Type::POSITION) {
    return false;
  }
  auto info_vec = frame_.getInformation();
  std::string info(info_vec.begin(), info_vec.end());

  if (info.length() < 27) {
    return false;
  }

  // leading '@' or '/'
  if (info.at(0) != '@' && info.at(0) != '/') {
    return false;
  }

  // time code
  position.time_code = info.substr(1, 7); // ddhhmmz

  // Symbol Table ID
  if (info.at(8) == '/') {
    position.symbol_table = aprs::Packet::SymbolTable::PRIMARY;
  } else if (info.at(8) == '\\') {
    position.symbol_table = aprs::Packet::SymbolTable::SECONDARY;
  } else {
    return false;
  }

  // Latitude
  std::string lat_str = info.substr(9, 4);
  int lat = base91Decode(std::vector<uint8_t>(lat_str.begin(), lat_str.end()));
  position.latitude = 90 - ((float)lat / 380926);

  // Longitude
  std::string lon_str = info.substr(13, 4);
  int lon = base91Decode(std::vector<uint8_t>(lon_str.begin(), lon_str.end()));
  position.longitude = ((float)lon / 190463) - 180;

  // Symbol
  position.symbol = info.at(17);

  // Course
  position.course = (info.at(18) - 33) * 4;

  // Speed
  const double speed_divisor = 0.076961;
  position.speed = std::exp((info.at(19) - 33) * speed_divisor) - 1;

  // Compression Type
  if (info.at(20) < 33) {
    return false;
  }

  // Altitude
  std::string alt_prefix = info.substr(21, 2);
  if (alt_prefix != "/A") {
    return false;
  }

  std::string alt_str = info.substr(24, 6);
  position.altitude = std::stoi(alt_str);

  // Comment
  if (info.length() > 30) {
    position.comment = info.substr(29);
  }

  return true;
}

} // namespace signal_easel::aprs