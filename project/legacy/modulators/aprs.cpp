#include <iostream>

#include "ax25.h"
#include "modulators.h"
#include "mwav.h"

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
  info.push_back('T'); // telemetry data
  info.push_back('#'); // digital telemetry
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