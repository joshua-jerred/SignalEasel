#include <cmath>
#include <iostream>
#include <vector>

#include "ax25.h"
#include "modulators.h"
#include "mwav-exception.h"
#include "mwav.h"

std::vector<uint8_t> base91Encode(int value, unsigned int num_bytes) {
  std::vector<uint8_t> encoded;

  for (int i = num_bytes - 1; i > -1; i--) {
    char c = value / (int)(std::pow(91, i));
    value = value % (int)(std::pow(91, i));
    encoded.push_back(c + 33);
  }

  if (encoded.size() != num_bytes) {
    throw std::runtime_error("Encoded value is not the correct size");
  }

  return encoded;
}

bool CheckPacketData(const mwav::AprsRequiredFields &packet) {
  if (packet.source_address.length() > 6 ||
      packet.source_address.length() < 3) {
    throw mwav::Exception("Invalid source address length");
  }

  if (packet.source_ssid > 15) {
    throw mwav::Exception("Invalid source ssid");
  }

  if (packet.destination_address.length() > 6 ||
      packet.destination_address.length() < 3) {
    throw mwav::Exception("Invalid destination address length");
  }

  if (packet.destination_ssid > 15) {
    throw mwav::Exception("Invalid destination ssid");
  }

  if (packet.symbol < '!' || packet.symbol > '~') {
    throw mwav::Exception("Invalid symbol");
  }

  return true;
}

bool CheckLocationData(
    const mwav::aprs_packet::CompressedPositionReport &location) {
  // time code
  if (location.time_code.length() != 6) {
    throw mwav::Exception("Invalid time code length");
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

bool AddRequiredFields(const mwav::AprsRequiredFields &required_fields,
                       AX25::Frame &frame) {
  try {
    if (!CheckPacketData(required_fields)) {
      return false;
    }
  } catch (mwav::Exception &e) {
    throw mwav::Exception("Invalid required fields: " + std::string(e.what()));
  }

  AX25::Address destination_address(required_fields.destination_address,
                                    required_fields.destination_ssid);
  AX25::Address source_address(required_fields.source_address,
                               required_fields.source_ssid, true);

  try {
    frame.AddAddress(destination_address);
    frame.AddAddress(source_address);
  } catch (mwav::Exception &e) {
    throw mwav::Exception("Invalid address: " + std::string(e.what()));
  }

  return true;
}

bool BuildFrame(WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
                std::vector<uint8_t> &info) {
  AX25::Frame frame;
  AddRequiredFields(required_fields, frame);
  frame.AddInformation(info);
  frame.BuildFrame();
  if (modulators::AfskEncodeBitStream(wavgen, frame.GetBitStream())) {
    return true;
  } else {
    return false;
  }
  return true;
}

bool AddLocationData(
    const mwav::aprs_packet::CompressedPositionReport &location,
    const mwav::AprsRequiredFields &required_fields,
    std::vector<uint8_t> &info) {
  bool valid = CheckLocationData(location);
  if (!valid) {
    throw mwav::Exception(
        "Invalid location data"); /** @todo generate an invalid frame */
  }

  info.push_back('@');  // telemetry data

  // time code
  for (char c : location.time_code) {
    info.push_back(c);
  }
  info.push_back('z');  // UTC

  // Symbol Table ID
  if (required_fields.symbol_table == mwav::AprsSymbolTable::PRIMARY) {
    info.push_back('/');
  } else if (required_fields.symbol_table == mwav::AprsSymbolTable::SECONDARY) {
    info.push_back('\\');
  } else {
    throw mwav::Exception("Invalid symbol table");
  }

  // Latitude
  int uncompressed_lat = (int)(380926 * (90 - location.latitude));
  std::vector<uint8_t> compressed_lat = base91Encode(uncompressed_lat, 4);
  for (int i = 0; i < 4; i++) {
    info.push_back(compressed_lat[i]);  // YYYY
  }

  // Longitude
  int uncompressed_lon = (int)(190463 * (180 + location.longitude));
  std::vector<uint8_t> compressed_lon = base91Encode(uncompressed_lon, 4);
  for (int i = 0; i < 4; i++) {
    info.push_back(compressed_lon[i]);  // XXXX
  }

  // Symbol
  info.push_back(required_fields.symbol);

  // csT
  uint8_t c = (location.course / 4) + 33;
  info.push_back(c);

  // Speed
  const double speed_divisor = 0.076961;
  uint8_t s = 0;
  if (location.speed > 0) {
    s = (int)std::round(std::log(location.speed + 1) / speed_divisor);
  }
  info.push_back(s + 33);  // s

  // Compression
  info.push_back((0b00111010 + 33));

  // Altitude
  int altitude = location.altitude;
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

  for (char c : location.comment) {
    info.push_back(c);
  }
  return true;
}

bool modulators::AprsEncodePositionPacket(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::CompressedPositionReport &location_data) {
  std::vector<uint8_t> info;
  AddLocationData(location_data, required_fields, info);
  return BuildFrame(wavgen, required_fields, info);
}

bool ValidateCoef(std::string coef) {
  // Basic validation, does not check if things are in the right places
  if (coef.size() < 1 || coef.size() > 9) {
    throw mwav::Exception("Invalid coef: " + coef);
  }
  for (char c : coef) {
    if ((c < '0' && c > '9') && (c != '-' && c != '.')) {
      throw mwav::Exception("Invalid character in coef: " + std::string(1, c));
    }
  }
  return true;
}

bool ValidateAnalogTelem(mwav::aprs_packet::Telemetry::Analog value) {
  if (value.name.length() < 1 || value.name.length() > value.upper_limit) {
    throw mwav::Exception("Invalid name: " + value.name);
  }

  if (value.unit.size() < 1 || value.unit.size() > value.upper_limit) {
    throw mwav::Exception("Invalid unit: " + value.unit);
  }

  if (!ValidateCoef(value.coef_a) || !ValidateCoef(value.coef_b) ||
      !ValidateCoef(value.coef_c)) {
    throw mwav::Exception("Invalid coef");
  }

  if (value.value.size() == 0 || value.value.size() > 3) {
    throw mwav::Exception("Invalid value: " + value.value);
  }

  return true;
}

bool ValidateDigitalTelem(mwav::aprs_packet::Telemetry::Digital data) {
  if (data.name.length() < 1 || data.name.length() > data.upper_limit) {
    throw mwav::Exception("Invalid name: " + data.name);
  }
  if (data.unit.size() < 1 || data.unit.size() > data.upper_limit) {
    throw mwav::Exception("Invalid unit: " + data.unit);
  }
  return true;
}

bool AddAnalogData(mwav::aprs_packet::Telemetry::Analog data,
                   std::vector<uint8_t> &info) {
  if (ValidateAnalogTelem(data)) {
    info.push_back(',');
    for (char c : data.value) {
      info.push_back(c);
    }
    return true;
  } else {
    return false;
  }
}

bool AddDigitalData(mwav::aprs_packet::Telemetry::Digital data,
                    std::vector<uint8_t> &info) {
  if (ValidateDigitalTelem(data)) {
    data.value ? info.push_back('1') : info.push_back('0');
    return true;
  } else {
    return false;
  }
}

bool modulators::AprsEncodeTelemetryData(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::Telemetry &telem) {
  std::vector<uint8_t> info;
  info.push_back('T');  // telemetry data
  info.push_back('#');  // digital telemetry
  if (telem.sequence_number.size() != 3) {
    return false;
  }
  for (char c : telem.sequence_number) {
    info.push_back(c);
  }

  if (!AddAnalogData(telem.A1, info) || !AddAnalogData(telem.A2, info) ||
      !AddAnalogData(telem.A3, info) || !AddAnalogData(telem.A4, info) ||
      !AddAnalogData(telem.A5, info)) {
    return false;
  }

  info.push_back(',');

  if (!AddDigitalData(telem.D1, info) || !AddDigitalData(telem.D2, info) ||
      !AddDigitalData(telem.D3, info) || !AddDigitalData(telem.D4, info) ||
      !AddDigitalData(telem.D5, info) || !AddDigitalData(telem.D6, info) ||
      !AddDigitalData(telem.D7, info) || !AddDigitalData(telem.D8, info)) {
    return false;
  }

  for (char c : telem.comment) {
    info.push_back(c);
  }

  return BuildFrame(wavgen, required_fields, info);
}

void AddTelemDestAddress(const mwav::aprs_packet::Telemetry &telem,
                         std::vector<uint8_t> &info) {
  std::string dest = telem.telem_destination_address;
  info.push_back(':');
  for (unsigned int i = 0; i < 9; i++) {
    if (i < dest.length()) {
      info.push_back(dest[i]);
    } else {
      info.push_back(' ');
    }
  }
  info.push_back(':');
}

bool AddParamName(mwav::aprs_packet::Telemetry::Analog data,
                  std::vector<uint8_t> &info, bool first = false) {
  if (!ValidateAnalogTelem(data)) {
    return false;
  }
  if (!first) {
    info.push_back(',');
  }
  for (char c : data.name) {
    info.push_back(c);
  }
  return true;
}

bool AddParamName(mwav::aprs_packet::Telemetry::Digital data,
                  std::vector<uint8_t> &info) {
  if (!ValidateDigitalTelem(data)) {
    return false;
  }
  info.push_back(',');
  for (char c : data.name) {
    info.push_back(c);
  }
  return true;
}

bool modulators::AprsEncodeTelemetryParamName(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::Telemetry &telemetry_data) {
  std::vector<uint8_t> info;
  AddTelemDestAddress(telemetry_data, info);
  info.push_back('P');
  info.push_back('A');
  info.push_back('R');
  info.push_back('M');
  info.push_back('.');

  if (!AddParamName(telemetry_data.A1, info, true) ||
      !AddParamName(telemetry_data.A2, info) ||
      !AddParamName(telemetry_data.A3, info) ||
      !AddParamName(telemetry_data.A4, info) ||
      !AddParamName(telemetry_data.A5, info) ||
      !AddParamName(telemetry_data.D1, info) ||
      !AddParamName(telemetry_data.D2, info) ||
      !AddParamName(telemetry_data.D3, info) ||
      !AddParamName(telemetry_data.D4, info) ||
      !AddParamName(telemetry_data.D5, info) ||
      !AddParamName(telemetry_data.D6, info) ||
      !AddParamName(telemetry_data.D7, info) ||
      !AddParamName(telemetry_data.D8, info)) {
    return false;
  }
  return BuildFrame(wavgen, required_fields, info);
}

bool AddParamUnitName(mwav::aprs_packet::Telemetry::Analog data,
                      std::vector<uint8_t> &info, bool first = false) {
  if (!ValidateAnalogTelem(data)) {
    return false;
  }
  if (!first) {
    info.push_back(',');
  }
  for (char c : data.unit) {
    info.push_back(c);
  }
  return true;
}

bool AddParamUnitName(mwav::aprs_packet::Telemetry::Digital data,
                      std::vector<uint8_t> &info, bool first = false) {
  if (!ValidateDigitalTelem(data)) {
    return false;
  }
  if (!first) {
    info.push_back(',');
  }
  for (char c : data.unit) {
    info.push_back(c);
  }
  return true;
}

bool modulators::AprsEncodeTelemetryParamUnits(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::Telemetry &telemetry_data) {
  std::vector<uint8_t> info;
  AddTelemDestAddress(telemetry_data, info);
  info.push_back('U');
  info.push_back('N');
  info.push_back('I');
  info.push_back('T');
  info.push_back('.');

  if (!AddParamUnitName(telemetry_data.A1, info, true) ||
      !AddParamUnitName(telemetry_data.A2, info) ||
      !AddParamUnitName(telemetry_data.A3, info) ||
      !AddParamUnitName(telemetry_data.A4, info) ||
      !AddParamUnitName(telemetry_data.A5, info) ||
      !AddParamUnitName(telemetry_data.D1, info) ||
      !AddParamUnitName(telemetry_data.D2, info) ||
      !AddParamUnitName(telemetry_data.D3, info) ||
      !AddParamUnitName(telemetry_data.D4, info) ||
      !AddParamUnitName(telemetry_data.D5, info) ||
      !AddParamUnitName(telemetry_data.D6, info) ||
      !AddParamUnitName(telemetry_data.D7, info) ||
      !AddParamUnitName(telemetry_data.D8, info)) {
    return false;
  }

  return BuildFrame(wavgen, required_fields, info);
}

bool AddParamCoefs(mwav::aprs_packet::Telemetry::Analog data,
                   std::vector<uint8_t> &info, bool first = false) {
  if (!ValidateAnalogTelem(data)) {
    return false;
  }
  if (!first) {
    info.push_back(',');
  }
  for (char c : data.coef_a) {
    info.push_back(c);
  }
  info.push_back(',');
  for (char c : data.coef_b) {
    info.push_back(c);
  }
  info.push_back(',');
  for (char c : data.coef_c) {
    info.push_back(c);
  }
  return true;
}

bool modulators::AprsEncodeTelemetryCoefs(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::Telemetry &telem) {
  std::vector<uint8_t> info;
  AddTelemDestAddress(telem, info);
  info.push_back('E');
  info.push_back('Q');
  info.push_back('N');
  info.push_back('S');
  info.push_back('.');
  if (!AddParamCoefs(telem.A1, info, true) || !AddParamCoefs(telem.A2, info) ||
      !AddParamCoefs(telem.A3, info) || !AddParamCoefs(telem.A4, info) ||
      !AddParamCoefs(telem.A5, info)) {
    return false;
  }
  return BuildFrame(wavgen, required_fields, info);
}

bool modulators::AprsEncodeTelemetryBitSenseProjName(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::Telemetry &telem) {
  std::vector<uint8_t> info;
  AddTelemDestAddress(telem, info);
  info.push_back('B');
  info.push_back('I');
  info.push_back('T');
  info.push_back('S');
  info.push_back('.');

  if (!AddDigitalData(telem.D1, info) || !AddDigitalData(telem.D2, info) ||
      !AddDigitalData(telem.D3, info) || !AddDigitalData(telem.D4, info) ||
      !AddDigitalData(telem.D5, info) || !AddDigitalData(telem.D6, info) ||
      !AddDigitalData(telem.D7, info) || !AddDigitalData(telem.D8, info)) {
    return false;
  }

  if (telem.project_title.size() > 0 && telem.project_title.size() <= 23) {
    info.push_back(',');
    for (char c : telem.project_title) {
      info.push_back(c);
    }
  }

  return BuildFrame(wavgen, required_fields, info);
}

bool ValidateMessage(const mwav::aprs_packet::Message &message_data) {
  if (message_data.addressee.size() > 9) {
    throw mwav::Exception("addressee too long");
  }
  if (message_data.message.size() > 67) {
    throw mwav::Exception("message too long");
  }
  if (message_data.message_id.size() > 5) {
    throw mwav::Exception("message_id too long");
  }

  for (char c : message_data.message) {
    if (c < 0x20 || c > 0x7e || c == '|' || c == '~' || c == '{') {
      throw mwav::Exception("invalid character in message");
    }
  }
  return true;
}

bool modulators::AprsEncodeMessage(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::Message &message_data) {
  if (!ValidateMessage(message_data)) {
    mwav::Exception("invalid message");
  }
  std::vector<uint8_t> info;
  info.push_back(':');
  int num_chars = 0;
  for (char c : message_data.addressee) {
    info.push_back(c);
    num_chars++;
  }
  for (int i = num_chars; i < 9; i++) {
    info.push_back(' ');
  }
  info.push_back(':');

  for (char c : message_data.message) {
    info.push_back(c);
  }

  if (message_data.message_id.size() > 0) {
    info.push_back('{');
    for (char c : message_data.message_id) {
      info.push_back(c);
    }
  }

  return BuildFrame(wavgen, required_fields, info);
}

bool modulators::AprsUserDefined(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::UserDefined &user_defined_data) {
  if (user_defined_data.data.size() > 252) {
    return false;
  }
  std::vector<uint8_t> info;
  info.push_back('{');
  info.push_back(user_defined_data.UserId);
  info.push_back(user_defined_data.UserDefPacketType);
  for (char c : user_defined_data.data) {
    info.push_back(c);
  }
  return BuildFrame(wavgen, required_fields, info);
}

bool modulators::AprsExperimental(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::Experimental &experimental_data) {
  if (experimental_data.data.size() > 253) {
    return false;
  }
  std::vector<uint8_t> info;
  info.push_back('{');
  info.push_back('{');
  for (char c : experimental_data.data) {
    info.push_back(c);
  }
  return BuildFrame(wavgen, required_fields, info);
}

bool modulators::AprsInvalidPacket(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::aprs_packet::Invalid &invalid_packet_data) {
  if (invalid_packet_data.data.size() > 254) {
    return false;
  }
  std::vector<uint8_t> info;
  info.push_back(',');
  for (uint8_t c : invalid_packet_data.data) {
    info.push_back(c);
  }
  return BuildFrame(wavgen, required_fields, info);
}