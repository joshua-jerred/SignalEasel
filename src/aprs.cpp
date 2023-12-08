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

#include "ax25.hpp"

namespace signal_easel {

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

bool CheckPacketData(const AprsPacket &packet) {
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

bool checkLocationData(const AprsPositionPacket &location) {
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

void AddRequiredFields(const AprsPacket &required_fields, Ax25Frame &frame) {

  CheckPacketData(required_fields);

  Ax25Frame::Address destination_address(required_fields.destination_address,
                                         required_fields.destination_ssid);
  Ax25Frame::Address source_address(required_fields.source_address,
                                    required_fields.source_ssid, true);
  frame.AddAddress(destination_address);
  frame.AddAddress(source_address);
}

std::vector<uint8_t> buildFrame(const AprsPacket &required_fields,
                                std::vector<uint8_t> &info) {
  Ax25Frame frame;
  AddRequiredFields(required_fields, frame);
  frame.AddInformation(info);

  auto vec = frame.BuildFrame();

  // frame.Print();

  return vec;
}

bool addLocationData(const AprsPositionPacket &packet,
                     const AprsPacket &required_fields,
                     std::vector<uint8_t> &info) {
  bool valid = checkLocationData(packet);
  if (!valid) {
    /** @todo generate an invalid frame */
    throw Exception(Exception::Id::APRS_INVALID_LOCATION_DATA);
  }

  info.push_back('@'); // telemetry data

  // time code
  for (char c : packet.time_code) {
    info.push_back(c);
  }
  info.push_back('z'); // UTC

  // Symbol Table ID
  if (required_fields.symbol_table == AprsPacket::SymbolTable::PRIMARY) {
    info.push_back('/');
  } else {
    info.push_back('\\');
  }

  // Latitude
  int uncompressed_lat = (int)(380926 * (90 - packet.latitude));
  std::vector<uint8_t> compressed_lat = base91Encode(uncompressed_lat, 4);
  for (int i = 0; i < 4; i++) {
    info.push_back(compressed_lat[i]); // YYYY
  }

  // Longitude
  int uncompressed_lon = (int)(190463 * (180 + packet.longitude));
  std::vector<uint8_t> compressed_lon = base91Encode(uncompressed_lon, 4);
  for (int i = 0; i < 4; i++) {
    info.push_back(compressed_lon[i]); // XXXX
  }

  // Symbol
  info.push_back(required_fields.symbol);

  // csT
  uint8_t c = (packet.course / 4) + 33;
  info.push_back(c);

  // Speed
  const double speed_divisor = 0.076961;
  uint8_t s = 0;
  if (packet.speed > 0) {
    s = (int)std::round(std::log(packet.speed + 1) / speed_divisor);
  }
  info.push_back(s + 33); // s

  // Compression
  info.push_back((0b00111010 + 33));

  // Altitude
  int altitude = packet.altitude;
  char alt[9] = {'/', 'A', '=', '0', '0', '0', '0', '0', '0'};
  if (altitude > 0 && altitude < 999999) {
    int i = 8;
    while (altitude > 0) {
      alt[i] = (altitude % 10) + '0';
      altitude /= 10;
      i--;
    }
  }

  for (int i = 0; i < 9; i++) {
    info.push_back(alt[i]);
  }

  for (char c : packet.comment) {
    info.push_back(c);
  }
  return true;
}

void AprsModulator::encodePositionPacket(AprsPositionPacket packet) {
  std::vector<uint8_t> info;
  addLocationData(packet, settings_.base_packet, info);
  std::vector<uint8_t> output_bytes = buildFrame(settings_.base_packet, info);

  // for (uint8_t byte : output_bytes) {
  //   std::cout << std::hex << static_cast<int>(byte) << " ";
  // }

  encodeBytes(output_bytes);
}

bool AprsDemodulator::lookForAx25Packet() {
  Ax25Frame frame;
  if (!frame.parseBitStream(output_bit_stream_)) {
    return false;
  }
  return true;
}

} // namespace signal_easel