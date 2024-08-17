/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   aprs_telemetry.hpp
 * @date   2024-08-16
 *
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#pragma once

#include <regex>

#include <SignalEasel/aprs.hpp>
#include <SignalEasel/exception.hpp>

#include <SignalEasel/aprs/telemetry_parameter.hpp>

namespace signal_easel::aprs::telemetry {

/// @brief A class to handle telemetry packets.
class TelemetryHandler {
public:
  TelemetryHandler() {}
  virtual ~TelemetryHandler() = default;

  void setAnalogValue(Parameter::Id id, uint8_t value) {
    if (Parameter::getType(id) != Parameter::Type::Analog) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setAnalogValue called with digital ID.");
    }
    analog_parameters_.at(static_cast<size_t>(id)).setRawValue(value);
  }

  uint8_t getAnalogValue(AnalogParameter::Id id) const {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getAnalogValue called with UNKNOWN ID.");
    }
    return analog_parameters_.at(static_cast<size_t>(id)).getValue();
  }

  void setAnalogName(AnalogParameter::Id id, const std::string &name) {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setAnalogName called with UNKNOWN ID.");
    }
    analog_parameters_.at(static_cast<size_t>(id)).setName(name);
  }

  std::string getAnalogName(AnalogParameter::Id id) const {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getAnalogName called with UNKNOWN ID.");
    }
    return analog_parameters_.at(static_cast<size_t>(id)).getName();
  }

  void setAnalogUnit(AnalogParameter::Id id, const std::string &unit) {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "setAnalogUnitOrLabel called with UNKNOWN ID.");
    }
    analog_parameters_.at(static_cast<size_t>(id)).setUnit(unit);
  }

  std::string getAnalogUnit(AnalogParameter::Id id) const {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "getAnalogUnitOrLabel called with UNKNOWN ID.");
    }
    return analog_parameters_.at(static_cast<size_t>(id)).getUnit();
  }

  void setAnalogCoefficients(AnalogParameter::Id id, const std::string &a,
                             const std::string &b, const std::string &c) {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "setAnalogCoefficients called with UNKNOWN ID.");
    }
    analog_parameters_.at(static_cast<size_t>(id)).setCoefficients(a, b, c);
  }

  void getAnalogCoefficients(AnalogParameter::Id id, std::string &a,
                             std::string &b, std::string &c) const {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "getAnalogCoefficients called with UNKNOWN ID.");
    }
    a = analog_parameters_.at(static_cast<size_t>(id)).getACoefficient();
    b = analog_parameters_.at(static_cast<size_t>(id)).getBCoefficient();
    c = analog_parameters_.at(static_cast<size_t>(id)).getCCoefficient();
  }

  void setDigitalValue(DigitalParameter::Id id, bool value) {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setDigitalValue called with UNKNOWN ID.");
    }
    digital_parameters_.at(static_cast<size_t>(id)).setValue(value);
  }

  bool getDigitalValue(DigitalParameter::Id id) const {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getDigitalValue called with UNKNOWN ID.");
    }
    return digital_parameters_.at(static_cast<size_t>(id)).getValue();
  }

  void setDigitalName(DigitalParameter::Id id, const std::string &name) {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setDigitalName called with UNKNOWN ID.");
    }
    digital_parameters_.at(static_cast<size_t>(id)).setName(name);
  }

  std::string getDigitalName(DigitalParameter::Id id) const {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getDigitalName called with UNKNOWN ID.");
    }
    return digital_parameters_.at(static_cast<size_t>(id)).getName();
  }

  void setDigitalLabel(DigitalParameter::Id id, const std::string &unit) {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "setDigitalUnitOrLabel called with UNKNOWN ID.");
    }
    digital_parameters_.at(static_cast<size_t>(id)).setLabel(unit);
  }

  std::string getDigitalLabel(DigitalParameter::Id id) const {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "getDigitalUnitOrLabel called with UNKNOWN ID.");
    }
    return digital_parameters_.at(static_cast<size_t>(id)).getLabel();
  }

  void setBitSense(DigitalParameter::Id id, bool sense) {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setBitSense called with UNKNOWN ID.");
    }
    digital_parameters_.at(static_cast<size_t>(id)).setSense(sense);
  }== DigitalParameter::Id::UNKNOWN) {
    throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                  "setBitSense called with UNKNOWN ID.");
  }
  digital_parameters_.at(static_cast<size_t>(id)).setSense(sense);
}

  bool getBitSense(DigitalParameter::Id id) const {
  if (id == DigitalParameter::Id::UNKNOWN) {
    throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                  "getBitSense called with UNKNOWN ID.");
  }
  return digital_parameters_.at(static_cast<size_t>(id)).getSense();
}

/// @brief Set the sequence number of the telemetry data report packets.
/// @param sequence_number - A value between 0 and 999. If the value is
/// outside of this range, it will be reset to 0.
bool setSequenceNumber(uint16_t sequence_number) {
  constexpr size_t SEQUENCE_NUMBER_MAX = 999;

  if (sequence_number > SEQUENCE_NUMBER_MAX) {
    sequence_number_ = 0;
    return false;
  }

  sequence_number_ = sequence_number;
  return true;
}

/// @brief Get the sequence number of the telemetry data report packets.
/// @return The sequence number of the telemetry data report packets.
uint16_t getSequenceNumber() const { return sequence_number_; }

/// @brief Set the comment of telemetry data report packets.
/// @param comment - A string of up to 220 characters, ASCII only but not
/// checked.
/// @return \c true if the comment was set, \c false if not.
bool setComment(const std::string &comment) {
  if (comment.size() > COMMENT_MAX_LENGTH_) {
    return false;
  }

  comment_ = comment;
  return true;
}

/// @brief Get the comment of telemetry data report packets.
std::string getComment() const { return comment_; }

/// @brief Set the project title of telemetry bit sense packets.
/// @param project_title - A string of up to 23 characters, ASCII only but not
/// @return \c true if the project title was set, \c false if not.
bool setProjectTitle(const std::string &project_title) {
  constexpr size_t PROJECT_TITLE_MAX_LENGTH = 22; // (23 - 1 for comma)
  if (project_title.size() > PROJECT_TITLE_MAX_LENGTH) {
    return false;
  }

  project_title_ = project_title;
  return true;
}

/// @brief Get the project title of telemetry bit sense packets.
std::string getProjectTitle() const { return project_title_; }

/// @brief Encode the telemetry report packet as a vector of bytes.
/// @return The packet as a vector of bytes.
std::vector<uint8_t> encodeDataReportMessage() {
  std::vector<uint8_t> packet;

  // Add the message type
  packet.push_back('T');

  // Add the sequence number
  packet.push_back('#');
  packet.push_back(sequence_number_ / 100 + '0');
  packet.push_back(sequence_number_ / 10 % 10 + '0');
  packet.push_back(sequence_number_ % 10 + '0');
  packet.push_back(',');

  // Add the analog parameters
  for (const auto &analog : analog_parameters_) {
    packet.push_back(analog.getValue() / 100 + '0');
    packet.push_back(analog.getValue() / 10 % 10 + '0');
    packet.push_back(analog.getValue() % 10 + '0');
    packet.push_back(',');
  }

  // Add the digital parameters
  for (const auto &digital : digital_parameters_) {
    packet.push_back(digital.getValue() ? '1' : '0');
  }

  // Add the comment
  for (char c : comment_) {
    packet.push_back(c);
  }

  return packet;
}

bool getBitSense(DigitalParameter::Id id) const {
  if (id == DigitalParameter::Id::UNKNOWN) {
    throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                  "getBitSense called with UNKNOWN ID.");
  }
  return digital_parameters_.at(static_cast<size_t>(id)).getSense();
}

/// @brief Set the sequence number of the telemetry data report packets.
/// @param sequence_number - A value between 0 and 999. If the value is
/// outside of this range, it will be reset to 0.
bool setSequenceNumber(uint16_t sequence_number) {
  constexpr size_t SEQUENCE_NUMBER_MAX = 999;

  if (sequence_number > SEQUENCE_NUMBER_MAX) {
    sequence_number_ = 0;
    return false;
  }

  sequence_number_ = sequence_number;
  return true;
}

/// @brief Get the sequence number of the telemetry data report packets.
/// @return The sequence number of the telemetry data report packets.
uint16_t getSequenceNumber() const { return sequence_number_; }

/// @brief Set the comment of telemetry data report packets.
/// @param comment - A string of up to 220 characters, ASCII only but not
/// checked.
/// @return \c true if the comment was set, \c false if not.
bool setComment(const std::string &comment) {
  if (comment.size() > COMMENT_MAX_LENGTH_) {
    return false;
  }

  comment_ = comment;
  return true;
}

/// @brief Get the comment of telemetry data report packets.
std::string getComment() const { return comment_; }

/// @brief Set the project title of telemetry bit sense packets.
/// @param project_title - A string of up to 23 characters, ASCII only but not
/// @return \c true if the project title was set, \c false if not.
bool setProjectTitle(const std::string &project_title) {
  constexpr size_t PROJECT_TITLE_MAX_LENGTH = 22; // (23 - 1 for comma)
  if (project_title.size() > PROJECT_TITLE_MAX_LENGTH) {
    return false;
  }

  project_title_ = project_title;
  return true;
}

/// @brief Get the project title of telemetry bit sense packets.
std::string getProjectTitle() const { return project_title_; }

/// @brief Encode the telemetry report packet as a vector of bytes.
/// @return The packet as a vector of bytes.
std::vector<uint8_t> encodeDataReportMessage() {
  std::vector<uint8_t> packet;

  // Add the message type
  packet.push_back('T');

  // Add the sequence number
  packet.push_back('#');
  packet.push_back(sequence_number_ / 100 + '0');
  packet.push_back(sequence_number_ / 10 % 10 + '0');
  packet.push_back(sequence_number_ % 10 + '0');
  packet.push_back(',');

  // Add the analog parameters
  for (const auto &analog : analog_parameters_) {
    packet.push_back(analog.getValue() / 100 + '0');
    packet.push_back(analog.getValue() / 10 % 10 + '0');
    packet.push_back(analog.getValue() % 10 + '0');
    packet.push_back(',');
  }

  // Add the digital parameters
  for (const auto &digital : digital_parameters_) {
    packet.push_back(digital.getValue() ? '1' : '0');
  }

  // Add the comment
  for (char c : comment_) {
    packet.push_back(c);
  }

  return packet;
}

bool decodeMessage(const std::vector<uint8_t> &message) {
  if (message.empty()) {
    return false;
  }

  switch (message.at(0)) {
  case 'T':
    return decodeDataReportMessage(message);
  case 'P':
    return decodeParameterDescriptor(message, true);
  case 'U':
    return decodeParameterDescriptor(message, false);
  case 'E':
    return decodeEquationCoefficientsMessage(message);
  case 'B':
    return decodeBitSenseProjectNameMessage(message);
  default:
    return false;
  };
}

private:
static constexpr size_t COMMENT_MAX_LENGTH_ = 220;

/// @brief Validate the first few bytes of a message.
/// @param message - The message to validate.
/// @param header - The header to validate against, ex: "PARM."
/// @return \c true if the header matches, \c false if not.
bool decodeValidateHeader(const std::vector<uint8_t> &message,
                          const std::string &header) {
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

uint16_t sequence_number_ = 0;
std::string comment_ = "";
std::string project_title_ = "";

std::array<AnalogParameter, 5> analog_parameters_{};
std::array<DigitalParameter, 8> digital_parameters_{};
};

} // namespace signal_easel::aprs::telemetry