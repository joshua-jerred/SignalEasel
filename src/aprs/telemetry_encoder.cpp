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
    std::vector<uint8_t> &output) const {
  output.clear();

  // Add the message type
  output.push_back('T');

  // Add the sequence number
  output.push_back('#');
  std::string sequence_number = getSequenceNumberString();
  for (char c : sequence_number) {
    output.push_back(c);
  }
  output.push_back(',');

  // Add the analog parameters
  for (const auto &analog : getAnalogParametersConst()) {
    output.push_back(analog.getValue() / 100 + '0');
    output.push_back(analog.getValue() / 10 % 10 + '0');
    output.push_back(analog.getValue() % 10 + '0');
    output.push_back(',');
  }

  // Add the digital parameters
  for (const auto &digital : getDigitalParametersConst()) {
    output.push_back(digital.getValue() ? '1' : '0');
  }

  // Add the comment
  for (char c : getComment()) {
    output.push_back(c);
  }

  return true;
}

bool TelemetryTranscoder::encodeParameterCoefficientMessage(
    std::vector<uint8_t> &output) const {
  output.clear();

  // Add the message type
  output.push_back('E');
  output.push_back('Q');
  output.push_back('N');
  output.push_back('S');
  output.push_back('.');

  // Add the equation coefficients for each analog parameter
  bool first = true;
  for (const auto &analog : getAnalogParametersConst()) {
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
    std::vector<uint8_t> &output) const {
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
  for (const auto &analog : getAnalogParametersConst()) {
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
  for (const auto &digital : getDigitalParametersConst()) {
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
    std::vector<uint8_t> &output) const {
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
  for (const auto &analog : getAnalogParametersConst()) {
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
  for (const auto &digital : getDigitalParametersConst()) {
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

bool TelemetryTranscoder::encodeBitSenseMessage(
    std::vector<uint8_t> &output) const {
  output.clear();

  // Add the message type
  output.push_back('B');
  output.push_back('I');
  output.push_back('T');
  output.push_back('S');
  output.push_back('.');

  // Add the digital parameters
  for (const auto &digital : getDigitalParametersConst()) {
    output.push_back(digital.getBitSense() ? '1' : '0');
  }

  std::string project_title = getProjectTitle();
  if (!project_title.empty()) {
    output.push_back(',');
    for (char c : project_title) {
      output.push_back(c);
    }
  }

  return true;
}

} // namespace signal_easel::aprs::telemetry