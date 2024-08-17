/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file       telemetry_data.hpp
 * @date       2024-08-17
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#pragma once

#include <array>

#include <SignalEasel/aprs/telemetry_parameter.hpp>

namespace signal_easel::aprs::telemetry {

/// @brief A container for telemetry data. Contains all of the components of
/// APRS telemetry messages.
class TelemetryData {
public:
  /// @brief Construct a new TelemetryData object.
  TelemetryData();
  /// @brief Destroy the TelemetryData object.
  virtual ~TelemetryData() = default;

  /// @brief Access an analog parameter.
  /// @param id - The ID of the parameter.
  /// @return A reference to the analog parameter.
  AnalogParameter &getAnalog(Parameter::Id id);

  /// @brief Access a digital parameter.
  /// @param id - The ID of the parameter.
  /// @return A reference to the digital parameter.
  DigitalParameter &getDigital(Parameter::Id id);

  /// @brief Set the sequence number. Must be less than 1000. If the sequence
  /// number is greater than 999, it will be set to 0.
  /// @param sequence_number - The sequence number.
  bool setSequenceNumber(uint16_t sequence_number);

  /// @brief Get the sequence number.
  /// @return The sequence number.
  uint16_t getSequenceNumber() const { return sequence_number_; }

  /// @brief Get the sequence number as a string of 3 digits.
  /// @return The sequence number as a string.
  std::string getSequenceNumberString() const;

  /// @brief Set the comment. Must be less than 220 characters. If the comment
  /// is too long, it will be truncated.
  /// @param comment - The comment.
  /// @return \c true if the comment was set, \c false if the comment was too
  /// long.
  bool setComment(const std::string &comment);

  /// @brief Get the comment.
  /// @return The comment.
  std::string getComment() const { return comment_; }

  /// @brief Set the project title. Must be less than 23 characters. If the
  /// project title is too long, it will be truncated.
  /// @param project_title - The project title.
  /// @return \c true if the project title was set, \c false if the project
  /// title was too long.
  bool setProjectTitle(const std::string &project_title);

  /// @brief Get the project title.
  /// @return The project title.
  std::string getProjectTitle() const { return project_title_; }

protected:
  const std::array<AnalogParameter, 5> &getAnalogParametersConst() const {
    return analog_parameters_;
  }

  std::array<AnalogParameter, 5> &getAnalogParameters() {
    return analog_parameters_;
  }

  const std::array<DigitalParameter, 8> &getDigitalParametersConst() const {
    return digital_parameters_;
  }

  std::array<DigitalParameter, 8> &getDigitalParameters() {
    return digital_parameters_;
  }

  /// @brief The maximum sequence number.
  static constexpr size_t SEQUENCE_NUMBER_MAX_ = 999;
  /// @brief The maximum length of the comment within the data report message.
  static constexpr size_t COMMENT_MAX_LENGTH_ = 220;
  /// @brief The maximum length of the project title within the bit sense
  /// message. This number is 23, but we need to account for the comma.
  static constexpr size_t PROJECT_TITLE_MAX_LENGTH_ = 23 - 1;

private:
  /// @brief The sequence number of the data report message.
  uint16_t sequence_number_{0};

  /// @brief The comment that is attached to the end of the data report message.
  std::string comment_{""};

  /// @brief The title of the project.
  std::string project_title_{""};

  /// @brief The analog parameters.
  std::array<AnalogParameter, 5> analog_parameters_;

  /// @brief The digital parameters.
  std::array<DigitalParameter, 8> digital_parameters_;
};

} // namespace signal_easel::aprs::telemetry