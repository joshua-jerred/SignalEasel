/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file       telemetry_transcoder.cpp
 * @date       2024-08-17
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/aprs/telemetry_transcoder.hpp>

namespace signal_easel::aprs::telemetry {

bool TelemetryTranscoder::encodeDataReportMessage(
    const TelemetryData &data, std::vector<uint8_t> &output) {
  output.clear();

  // Add the message type
  output.push_back('T');

  // Add the sequence number
  output.push_back('#');
  std::string sequence_number = data.getSequenceNumberString();
  for (char c : sequence_number) {
    output.push_back(c);
  }
  output.push_back(',');

  // Add the analog parameters
  for (const auto &analog : data.getAnalogParametersConst()) {
    output.push_back(analog.getValue() / 100 + '0');
    output.push_back(analog.getValue() / 10 % 10 + '0');
    output.push_back(analog.getValue() % 10 + '0');
    output.push_back(',');
  }

  // Add the digital parameters
  for (const auto &digital : data.getDigitalParametersConst()) {
    output.push_back(digital.getValue() ? '1' : '0');
  }

  // Add the comment
  for (char c : data.getComment()) {
    output.push_back(c);
  }

  return true;
}

bool TelemetryTranscoder::encodeParameterCoefficientMessage(
    const TelemetryData &data, std::vector<uint8_t> &output) {
  output.clear();

  // Add the message type
  output.push_back('E');
  output.push_back('Q');
  output.push_back('N');
  output.push_back('S');
  output.push_back('.');

  // Add the equation coefficients for each analog parameter
  bool first = true;
  for (const auto &analog : data.getAnalogParametersConst()) {
    if (!first) {
      output.push_back(',');
    }
    first = false;

    for (char c : analog.getCoefficientA()) {
      output.push_back(c);
    }
    output.push_back(',');

    for (char c : analog.getCoefficientB()) {
      output.push_back(c);
    }
    output.push_back(',');

    for (char c : analog.getCoefficientC()) {
      output.push_back(c);
    }
  }

  return true;
}

bool TelemetryTranscoder::encodeParameterNameMessage(
    const TelemetryData &data, std::vector<uint8_t> &output) {
  output.clear();

  // Add the message type
  output.push_back('P');
  output.push_back('A');
  output.push_back('R');
  output.push_back('M');
  output.push_back('.');

  // Store a position to the last parameter with a name
  size_t last_param_with_name_pos = output.size();

  // Add the analog parameters
  bool first = true;
  for (const auto &analog : data.getAnalogParametersConst()) {
    if (!first) {
      output.push_back(',');
    }
    first = false;

    for (char c : analog.getName()) {
      output.push_back(c);
    }

    if (!analog.getName().empty()) {
      last_param_with_name_pos = output.size();
    }
  }

  // Add the digital parameters
  for (const auto &digital : data.getDigitalParametersConst()) {
    output.push_back(',');

    for (char c : digital.getName()) {
      output.push_back(c);
    }

    if (!digital.getName().empty()) {
      last_param_with_name_pos = output.size();
    }
  }

  // Trim the output to the last parameter with a name
  output.resize(last_param_with_name_pos);

  return true;
}

bool TelemetryTranscoder::encodeUnitAndLabelMessage(
    const TelemetryData &data, std::vector<uint8_t> &output) {
  output.clear();

  // Add the message type
  output.push_back('U');
  output.push_back('N');
  output.push_back('I');
  output.push_back('T');
  output.push_back('.');

  // Store a position to the last parameter with a name
  size_t last_param_with_name_pos = output.size();

  // Add the analog parameters
  bool first = true;
  for (const auto &analog : data.getAnalogParametersConst()) {
    if (!first) {
      output.push_back(',');
    }
    first = false;

    for (char c : analog.getLabel()) {
      output.push_back(c);
    }

    if (!analog.getLabel().empty()) {
      last_param_with_name_pos = output.size();
    }
  }

  // Add the digital parameters
  for (const auto &digital : data.getDigitalParametersConst()) {
    output.push_back(',');

    for (char c : digital.getLabel()) {
      output.push_back(c);
    }

    if (!digital.getLabel().empty()) {
      last_param_with_name_pos = output.size();
    }
  }

  // Trim the output to the last parameter with a name
  output.resize(last_param_with_name_pos);

  return true;
}

bool TelemetryTranscoder::encodeBitSenseMessage(const TelemetryData &data,
                                                std::vector<uint8_t> &output) {
  output.clear();

  // Add the message type
  output.push_back('B');
  output.push_back('I');
  output.push_back('T');
  output.push_back('S');
  output.push_back('.');

  // Add the digital parameters
  for (const auto &digital : data.getDigitalParametersConst()) {
    output.push_back(digital.getBitSense() ? '1' : '0');
  }

  std::string project_title = data.getProjectTitle();
  if (!project_title.empty()) {
    output.push_back(',');
    for (char c : project_title) {
      output.push_back(c);
    }
  }

  return true;
}

bool TelemetryTranscoder::decodeMessage(TelemetryData &data,
                                        const std::vector<uint8_t> &message) {
  if (message.empty()) {
    return false;
  }

  switch (message.at(0)) {
  case 'T':
    return decodeDataReportMessage(data, message);
  case 'P':
    return decodeParameterDescriptor(data, message, true);
  case 'U':
    return decodeParameterDescriptor(data, message, false);
  case 'E':
    return decodeParameterCoefficientMessage(data, message);
  case 'B':
    return decodeBitSenseMessage(data, message);
  default:
    return false;
  };
}

bool TelemetryTranscoder::decodeValidateHeader(
    const std::vector<uint8_t> &message, const std::string &header) {
  if (message.size() < header.size()) {
    return false;
  }

  for (size_t i = 0; i < header.size(); i++) {
    if (message.at(i) != header.at(i)) {
      return false;
    }
  }

  return true;
}

bool TelemetryTranscoder::decodeDataReportMessage(
    TelemetryData &data, const std::vector<uint8_t> &message) {
  constexpr size_t MINIMUM_DATA_REPORT_MESSAGE_LENGTH =
      33; // In the case of "MIC" and not comma following it.
  if (message.size() < MINIMUM_DATA_REPORT_MESSAGE_LENGTH) {
    return false;
  }

  // Helper to check if a character is a digit
  auto isDigit = [](uint8_t c) { return c >= '0' && c <= '9'; };

  size_t pos = 0;

  // Check for T#
  if (message.at(pos++) != 'T' || message.at(pos++) != '#') {
    return false;
  }

  // Get the sequence number or MIC
  if (message.at(pos) == 'M' && message.at(pos + 1) == 'I' &&
      message.at(pos + 2) == 'C') {
    data.setSequenceNumber(0); // MIC, just set to 0
    pos += 3;

    // MIC may or may not have a comma after it
    if (message.at(pos) != ',') {
      pos++;
    }
  } else {
    if (!isDigit(message.at(pos)) || !isDigit(message.at(pos + 1)) ||
        !isDigit(message.at(pos + 2)) || message.at(pos + 3) != ',') {
      return false;
    }

    uint16_t sequence_num = (message.at(pos) - '0') * 100 +
                            (message.at(pos + 1) - '0') * 10 +
                            (message.at(pos + 2) - '0');
    data.setSequenceNumber(sequence_num);

    pos += 4;
  }

  // Get the analog parameters
  for (auto &analog : data.getAnalogParameters()) {

    // Ensure the next three characters are digits
    if (!isDigit(message.at(pos)) || !isDigit(message.at(pos + 1)) ||
        !isDigit(message.at(pos + 2))) {
      return false;
    }

    analog.setRawValue((message.at(pos) - '0') * 100 +
                       (message.at(pos + 1) - '0') * 10 +
                       (message.at(pos + 2) - '0'));

    pos += 3;

    // Trailing comma
    if (message.at(pos++) != ',') {
      return false;
    }
  }

  // Get the digital parameters
  for (auto &digital : data.getDigitalParameters()) {
    uint8_t bit_char = message.at(pos++);
    if (bit_char == '1') {
      digital.setValue(true);
    } else if (bit_char == '0') {
      digital.setValue(false);
    } else {
      return false;
    }
  }

  // Get the comment, if it exists
  std::string comment = "";
  while (pos < message.size() &&
         pos < MINIMUM_DATA_REPORT_MESSAGE_LENGTH + data.COMMENT_MAX_LENGTH_) {
    comment += message.at(pos++);
  }
  data.setComment(comment);

  return true;
}

bool TelemetryTranscoder::decodeParameterDescriptor(
    TelemetryData &data, const std::vector<uint8_t> &message,
    const bool name_or_unit) {
  constexpr size_t MINIMUM_PARAMETER_NAME_OR_UNIT_MESSAGE_LENGTH =
      5 + 1; // PARM.x

  if (message.size() < MINIMUM_PARAMETER_NAME_OR_UNIT_MESSAGE_LENGTH) {
    return false;
  }

  const std::string EXPECTED_HEADER = name_or_unit ? "PARM." : "UNIT.";
  if (!decodeValidateHeader(message, EXPECTED_HEADER)) {
    return false;
  }

  size_t pos = EXPECTED_HEADER.size();

  // Make sure that the first parameter has a value and isn't skipped
  if (message.at(pos) == ',') {
    return false;
  }

  // First attempt to decode the analog parameter names
  for (auto &analog : data.getAnalogParameters()) {
    // If there is data available, read it, otherwise the descriptor is empty
    // which is expected.
    std::string descriptor = "";
    while (pos < message.size()) {
      // Check for a comma
      if (message.at(pos) == ',') {
        pos++;
        break;
      }

      descriptor += message.at(pos++);
    }

    name_or_unit ? analog.setName(descriptor)
                 : analog.setUnitOrLabel(descriptor);
  }

  // Now do the same for the digital parameters
  for (auto &digital : data.getDigitalParameters()) {
    // If there is data available, read it, otherwise the descriptor is empty
    // which is expected.
    std::string descriptor = "";
    while (pos < message.size()) {
      // Check for a comma
      if (message.at(pos) == ',') {
        pos++;
        break;
      }

      descriptor += message.at(pos++);
    }

    name_or_unit ? digital.setName(descriptor)
                 : digital.setUnitOrLabel(descriptor);
  }

  return true;
}

bool TelemetryTranscoder::decodeParameterCoefficientMessage(
    TelemetryData &data, const std::vector<uint8_t> &message) {
  constexpr size_t MINIMUM_EQUATION_COEFFICIENTS_MESSAGE_LENGTH = 5 + 29;
  if (message.size() < MINIMUM_EQUATION_COEFFICIENTS_MESSAGE_LENGTH) {
    return false;
  }

  const std::string EXPECTED_HEADER = "EQNS.";
  if (!decodeValidateHeader(message, EXPECTED_HEADER)) {
    return false;
  }

  size_t pos = EXPECTED_HEADER.size();

  // Get the equation coefficients for each analog parameter
  for (auto &analog : data.getAnalogParameters()) {
    std::string a = "";
    std::string b = "";
    std::string c = "";

    // Get the A coefficient
    while (pos < message.size() && message.at(pos) != ',') {
      a += message.at(pos++);
    }

    if (!validateCoefficient(a)) {
      return false;
    }

    // Skip the comma
    pos++;

    // Get the B coefficient
    while (pos < message.size() && message.at(pos) != ',') {
      b += message.at(pos++);
    }

    if (!validateCoefficient(b)) {
      return false;
    }

    // Skip the comma
    pos++;

    // Get the C coefficient
    while (pos < message.size() && message.at(pos) != ',') {
      c += message.at(pos++);
    }

    if (!validateCoefficient(c)) {
      return false;
    }

    // Skip the comma
    pos++;

    analog.setCoefficients(a, b, c);
  }

  return true;
}

bool TelemetryTranscoder::decodeBitSenseMessage(
    TelemetryData &data, const std::vector<uint8_t> &message) {
  constexpr size_t MINIMUM_BIT_SENSE_MESSAGE_LENGTH = 5 + 8;
  if (message.size() < MINIMUM_BIT_SENSE_MESSAGE_LENGTH) {
    return false;
  }

  const std::string EXPECTED_HEADER = "BITS.";
  if (!decodeValidateHeader(message, EXPECTED_HEADER)) {
    return false;
  }

  size_t pos = EXPECTED_HEADER.size();

  // Get the bit sense values
  for (auto &digital : data.getDigitalParameters()) {
    if (pos >= message.size()) {
      return false;
    }

    if (message.at(pos) == '1') {
      digital.setBitSense(true);
    } else if (message.at(pos) == '0') {
      digital.setBitSense(false);
    } else {
      return false;
    }

    pos++;
  }

  // Get the project title if it exists
  if (pos < message.size()) {
    std::string project_title = "";

    // Not clearly defined in the spec, but the example shows a comma
    if (message.at(pos) == ',') {
      pos++;
    }

    for (; pos < message.size(); pos++) {
      project_title += message.at(pos);
    }

    data.setProjectTitle(project_title);
  }

  return true;
}

} // namespace signal_easel::aprs::telemetry