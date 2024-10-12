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
  if (info.empty()) {
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
  case ':': {
    // Telemetry messages that describe the telemetry data report are in the
    // APRS message format. Ex: ":NOCALL-1 :BITS.xxx"
    if (info.size() > 15 && info.at(15) == '.') {
      std::string message_type(info.begin() + 11, info.begin() + 15);
      if (message_type == "PARM") {
        type_ = aprs::Packet::Type::TELEMETRY_PARAMETER_NAME;
      } else if (message_type == "UNIT") {
        type_ = aprs::Packet::Type::TELEMETRY_PARAMETER_UNIT;
      } else if (message_type == "EQNS") {
        type_ = aprs::Packet::Type::TELEMETRY_COEFFICIENT;
      } else if (message_type == "BITS") {
        type_ = aprs::Packet::Type::TELEMETRY_BIT_SENSE_PROJ_NAME;
      } else {
        type_ = aprs::Packet::Type::MESSAGE;
      }
    } else {
      type_ = aprs::Packet::Type::MESSAGE;
    }
  } break;
  case '{':
    type_ = aprs::Packet::Type::EXPERIMENTAL;
    break;
  case 'T': // TELEMETRY DATA REPORT
    type_ = aprs::Packet::Type::TELEMETRY_DATA_REPORT;
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
  populateGenericFields(message_packet);

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
  populateGenericFields(position);

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
  position.time_code = info.substr(1, 6); // ddhhmmz, ignore the z

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

bool Demodulator::parseExperimentalPacket(
    aprs::ExperimentalPacket &experimental) {
  if (type_ != aprs::Packet::Type::EXPERIMENTAL) {
    /// @todo assert here or something instead. Same with other parse functions
    return false;
  }

  populateGenericFields(experimental);

  auto info = frame_.getInformation();
  if (info.size() < 3) {
    return false;
  }

  // leading '{{'
  if (info.at(0) != '{' || info.at(1) != '{') {
    return false;
  }

  experimental.packet_type_char = info.at(2);

  if (info.size() < 4) {
    experimental.data = std::vector<uint8_t>();
    return true;
  }
  experimental.data = std::vector<uint8_t>(info.begin() + 3, info.end());
  return true;
}

bool Demodulator::parseTelemetryPacket(aprs::TelemetryPacket &packet) {
  if (type_ != aprs::Packet::Type::TELEMETRY_DATA_REPORT &&
      type_ != aprs::Packet::Type::TELEMETRY_PARAMETER_NAME &&
      type_ != aprs::Packet::Type::TELEMETRY_PARAMETER_UNIT &&
      type_ != aprs::Packet::Type::TELEMETRY_COEFFICIENT &&
      type_ != aprs::Packet::Type::TELEMETRY_BIT_SENSE_PROJ_NAME) {
    /// @todo assert here or something instead. Same with other parse functions
    return false;
  }
  populateGenericFields(packet);

  packet.telemetry_type = type_;

  auto &info = frame_.getInformationRef();
  telemetry::TelemetryTranscoder decoder{};

  return decoder.decodeMessage(packet.telemetry_data, info);
}

void Demodulator::populateGenericFields(aprs::Packet &packet) const {
  const auto source = frame_.getSourceAddress();
  packet.source_address = source.getAddressString();
  packet.source_ssid = source.getSsid();

  const auto destination = frame_.getDestinationAddress();
  packet.destination_ssid = destination.getSsid();
  packet.destination_address = destination.getAddressString();
}

} // namespace signal_easel::aprs