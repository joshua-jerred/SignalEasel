/**
 * @file aprs_encoders.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Encoding functions for APRS packets.
 * @date 2024-01-14
 * @copyright Copyright (c) 2024
 */

#include <cmath>

#include <SignalEasel/aprs.hpp>
#include <SignalEasel/exception.hpp>

namespace signal_easel::aprs {

std::vector<uint8_t> PositionPacket::encode() const {
  std::vector<uint8_t> info;

  /// @warning CHECK LOCATION DATA VALIDITY
  /// @todo Check location data validity

  info.push_back('@'); // telemetry data

  // time code
  for (char c : time_code) {
    info.push_back(c);
  }
  info.push_back('z'); // UTC DDHHMM

  // Symbol Table ID
  if (symbol_table == aprs::Packet::SymbolTable::PRIMARY) {
    info.push_back('/');
  } else {
    info.push_back('\\');
  }

  // Latitude
  int uncompressed_lat = (int)(380926 * (90 - latitude));
  std::vector<uint8_t> compressed_lat = base91Encode(uncompressed_lat, 4);
  for (int i = 0; i < 4; i++) {
    info.push_back(compressed_lat[i]); // YYYY
  }

  // Longitude
  int uncompressed_lon = (int)(190463 * (180 + longitude));
  std::vector<uint8_t> compressed_lon = base91Encode(uncompressed_lon, 4);
  for (int i = 0; i < 4; i++) {
    info.push_back(compressed_lon[i]); // XXXX
  }

  // Symbol
  info.push_back(symbol);

  // csT
  uint8_t encoded_course = (course / 4) + 33;
  info.push_back(encoded_course);

  // Speed
  const double speed_divisor = 0.076961;
  uint8_t s = 0;
  if (speed > 0) {
    s = (int)std::round(std::log(speed + 1) / speed_divisor);
  }
  info.push_back(s + 33); // speed

  // Compression
  info.push_back((0b00111010 + 33));

  // Altitude
  int alt_buffer = altitude;
  char alt[9] = {'/', 'A', '=', '0', '0', '0', '0', '0', '0'};
  if (alt_buffer > 0 && alt_buffer < 999999) {
    int i = 8;
    while (alt_buffer > 0) {
      alt[i] = (alt_buffer % 10) + '0';
      alt_buffer /= 10;
      i--;
    }
  }

  for (int i = 0; i < 9; i++) {
    info.push_back(alt[i]);
  }

  for (char c : comment) {
    info.push_back(c);
  }

  /// @todo Bearing and Number/Range/Quality

  return info;
}

std::vector<uint8_t> MessagePacket::encode() const {
  std::vector<uint8_t> info;

  // Addressee
  // if (addressee.length() > 9) {
  // throw Exception(Exception::Id::APRS_INVALID_MESSAGE_ADDRESSEE_LENGTH);
  // }
  info.push_back(':');
  for (char c : addressee) {
    info.push_back(c);
  }
  for (size_t i = addressee.length(); i < 9; i++) {
    info.push_back(' ');
  }
  info.push_back(':');

  // Message
  for (char c : message) {
    info.push_back(c);
  }

  // Message ID
  info.push_back('{');
  if (!message_id.empty()) {
    for (char c : message_id) {
      info.push_back(c);
    }
  }

  return info;
}

std::vector<uint8_t> MessageAck::encode() const {
  std::vector<uint8_t> info;

  info.push_back(':');
  for (char c : addressee) {
    info.push_back(c);
  }
  for (size_t i = addressee.length(); i < 9; i++) {
    info.push_back(' ');
  }
  info.push_back(':');
  info.push_back('a');
  info.push_back('c');
  info.push_back('k');
  for (char c : message_id) {
    info.push_back(c);
  }
  return info;
}

void addAnalogValue(uint8_t value, std::vector<uint8_t> &info) {
  std::string str = std::to_string(value);
  while (str.length() < 3) {
    str = "0" + str;
  }
  for (char c : str) {
    info.push_back(c);
  }
  info.push_back(',');
}

void addUnitOrParam(const std::string &unit_or_param,
                    std::vector<uint8_t> &info, size_t max_length,
                    bool first = false) {
  if (!first) {
    info.push_back(',');
  }
  int len = unit_or_param.length();
  if (len == 0) {
    info.push_back(' ');
    return;
  }
  size_t i = 0;
  while (i < max_length && len > 0) {
    info.push_back(unit_or_param.at(i));
    i++;
    len--;
  }
}

// std::vector<uint8_t> Telemetry::encode(TelemetryPacketType type) const {
//   std::vector<uint8_t> info;
//   if (type == TelemetryPacketType::DATA_REPORT) {
//     // T#
//     info.push_back('T');
//     info.push_back('#');

//     // Sequence Number
//     int i = 3;
//     auto seq_len = sequence_number.length();
//     while (i > 0 && seq_len > 0) {
//       info.push_back(sequence_number.at(seq_len - 1));
//       seq_len--;
//       i--;
//     }
//     while (i > 0) {
//       info.push_back('0');
//       i--;
//     }
//     info.push_back(',');

//     // Analog Values
//     addAnalogValue(a1.value, info);
//     addAnalogValue(a2.value, info);
//     addAnalogValue(a3.value, info);
//     addAnalogValue(a4.value, info);
//     addAnalogValue(a5.value, info);

//     // Digital Values
//     info.push_back(d1.value ? '1' : '0');
//     info.push_back(d2.value ? '1' : '0');
//     info.push_back(d3.value ? '1' : '0');
//     info.push_back(d4.value ? '1' : '0');
//     info.push_back(d5.value ? '1' : '0');
//     info.push_back(d6.value ? '1' : '0');
//     info.push_back(d7.value ? '1' : '0');
//     info.push_back(d8.value ? '1' : '0');

//     // Comment
//     if (info.size() + comment.length() > 220) {
//       throw Exception(Exception::Id::APRS_INVALID_COMMENT_LENGTH);
//     }
//     for (char c : comment) {
//       info.push_back(c);
//     }

//   } else if (type == TelemetryPacketType::PARAM_NAME) {

//     info = std::vector<uint8_t>({'P', 'A', 'R', 'M', '.'});
//     // the max length is one less than spec as a comma is added.
//     addUnitOrParam(a1.name, info, 7, true);
//     addUnitOrParam(a2.name, info, 6);
//     addUnitOrParam(a3.name, info, 5);
//     addUnitOrParam(a4.name, info, 5);
//     addUnitOrParam(a5.name, info, 4);
//     addUnitOrParam(d1.name, info, 5);
//     addUnitOrParam(d2.name, info, 4);
//     addUnitOrParam(d3.name, info, 3);
//     addUnitOrParam(d4.name, info, 3);
//     addUnitOrParam(d5.name, info, 3);
//     addUnitOrParam(d6.name, info, 2);
//     addUnitOrParam(d7.name, info, 2);
//     addUnitOrParam(d8.name, info, 2);
//     return info;

//   } else if (type == TelemetryPacketType::PARAM_UNIT) {

//     info = std::vector<uint8_t>({'U', 'N', 'I', 'T', '.'});
//     // the max length is one less than spec as a comma is added.
//     addUnitOrParam(a1.unit, info, 7, true);
//     addUnitOrParam(a2.unit, info, 6);
//     addUnitOrParam(a3.unit, info, 5);
//     addUnitOrParam(a4.unit, info, 5);
//     addUnitOrParam(a5.unit, info, 4);
//     addUnitOrParam(d1.unit, info, 5);
//     addUnitOrParam(d2.unit, info, 4);
//     addUnitOrParam(d3.unit, info, 3);
//     addUnitOrParam(d4.unit, info, 3);
//     addUnitOrParam(d5.unit, info, 3);
//     addUnitOrParam(d6.unit, info, 2);
//     addUnitOrParam(d7.unit, info, 2);
//     addUnitOrParam(d8.unit, info, 2);
//     return info;

//   } else if (type == TelemetryPacketType::PARAM_COEF) {

//     info = std::vector<uint8_t>({'E', 'Q', 'N', 'S', '.'});
//     std::vector<std::string> coefs = {
//         a1.coef_a, a1.coef_b, a1.coef_c, a2.coef_a, a2.coef_b,
//         a2.coef_c, a3.coef_a, a3.coef_b, a3.coef_c, a4.coef_a,
//         a4.coef_b, a4.coef_c, a5.coef_a, a5.coef_b, a5.coef_c};
//     bool first = true;
//     for (const std::string &coef : coefs) {
//       if (!first) {
//         info.push_back(',');
//       }
//       first = false;
//       for (char c : coef) {
//         info.push_back(c);
//       }
//     }
//     return info;

//   } else if (type == TelemetryPacketType::BIT_SENSE_PROJ_NAME) {

//     info = std::vector<uint8_t>({'B', 'I', 'T', 'S', '.'});
//     info.push_back(d1.sense ? '1' : '0');
//     info.push_back(d2.sense ? '1' : '0');
//     info.push_back(d3.sense ? '1' : '0');
//     info.push_back(d4.sense ? '1' : '0');
//     info.push_back(d5.sense ? '1' : '0');
//     info.push_back(d6.sense ? '1' : '0');
//     info.push_back(d7.sense ? '1' : '0');
//     info.push_back(d8.sense ? '1' : '0');

//     // Project Title
//     int i = 0;
//     for (char c : project_title) {
//       info.push_back(c);
//       i++;
//       if (i == 23) {
//         break;
//       }
//     }
//   }
//   return info;
// }

std::vector<uint8_t> Experimental::encode() const {
  std::vector<uint8_t> info;
  info.push_back('{');
  info.push_back('{');
  info.push_back(packet_type_char);
  for (uint8_t data_byte : data) {
    info.push_back(data_byte);
  }
  return info;
}

} // namespace signal_easel::aprs
