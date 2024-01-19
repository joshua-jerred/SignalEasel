/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   aprs.cpp
 * @date   2023-12-05
 * @brief  APRS implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <cmath>
#include <stdexcept>
#include <vector>

#include <SignalEasel/aprs.hpp>
#include <SignalEasel/exception.hpp>

namespace signal_easel::aprs {

std::vector<uint8_t> base91Encode(int value, unsigned int num_bytes) {
  std::vector<uint8_t> encoded;

  for (int i = num_bytes - 1; i > -1; i--) {
    char c = value / (int)(std::pow(91, i));
    value = value % (int)(std::pow(91, i));
    encoded.push_back(c + 33);
  }

  if (encoded.size() != num_bytes) {
    throw Exception(Exception::Id::BASE_91_ENCODE_VALUE_NOT_CORRECT_SIZE);
  }

  return encoded;
}

int base91Decode(std::vector<uint8_t> encoded) {
  int value = 0;
  for (size_t i = 0; i < encoded.size(); i++) {
    value += (encoded.at(i) - 33) * (int)(std::pow(91, encoded.size() - i - 1));
  }
  return value;
}

bool CheckPacketData(const aprs::Packet &packet) {
  if (packet.source_address.length() > 6 ||
      packet.source_address.length() < 3) {
    throw Exception(Exception::Id::APRS_INVALID_SOURCE_ADDRESS_LENGTH);
  }

  if (packet.source_ssid > 15) {
    throw Exception(Exception::Id::APRS_INVALID_SOURCE_SSID);
  }

  if (packet.destination_address.length() > 6 ||
      packet.destination_address.length() < 3) {
    throw Exception(Exception::Id::APRS_INVALID_DESTINATION_ADDRESS_LENGTH);
  }

  if (packet.destination_ssid > 15) {
    throw Exception(Exception::Id::APRS_INVALID_DESTINATION_SSID);
  }

  if (packet.symbol < '!' || packet.symbol > '~') {
    throw Exception(Exception::Id::APRS_INVALID_SYMBOL);
  }

  return true;
}

bool checkLocationData(const aprs::PositionPacket &location) {
  // time code
  if (location.time_code.length() != 6) {
    throw Exception(Exception::Id::APRS_LOCATION_INVALID_TIME_CODE_LENGTH);
  }

  std::string hh = location.time_code.substr(0, 2);
  std::string mm = location.time_code.substr(2, 2);
  std::string ss = location.time_code.substr(4, 2);

  try {
    if (std::stoi(hh) < 0 || std::stoi(hh) > 23) {
      return false;
    }
    if (std::stoi(mm) < 0 || std::stoi(mm) > 59) {
      return false;
    }
    if (std::stoi(ss) < 0 || std::stoi(ss) > 59) {
      return false;
    }
  } catch (std::invalid_argument &e) {
    return false;
  }

  if (location.latitude < -90 || location.latitude > 90) {
    return false;
  }
  if (location.longitude < -180 || location.longitude > 180) {
    return false;
  }
  // the dead sea to an arbitrarily high altitude
  if (location.altitude < -1400 || location.altitude > 150000) {
    return false;
  }
  if (location.speed < 0 || location.speed > 400) {
    return false;
  }
  if (location.course < 0 || location.course > 360) {
    return false;
  }
  return true;
}

void AddRequiredFields(const aprs::Packet &required_fields,
                       ax25::Frame &frame) {

  CheckPacketData(required_fields);

  ax25::Address destination_address(required_fields.destination_address,
                                    required_fields.destination_ssid);
  ax25::Address source_address(required_fields.source_address,
                               required_fields.source_ssid, true);
  frame.setDestinationAddress(destination_address);
  frame.setSourceAddress(source_address);
}

std::vector<uint8_t> encodePacket(const aprs::Packet &required_fields,
                                  std::vector<uint8_t> &info) {
  ax25::Frame frame;
  AddRequiredFields(required_fields, frame);
  frame.setInformation(info);

  auto vec = frame.encodeFrame();

  return vec;
}

} // namespace signal_easel::aprs