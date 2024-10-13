/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file       telemetry_data.cpp
 * @date       2024-08-17
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/aprs/telemetry_data.hpp>
#include <SignalEasel/exception.hpp>

namespace signal_easel::aprs::telemetry {

TelemetryData::TelemetryData()
    : analog_parameters_{AnalogParameter(Parameter::Id::A1),
                         AnalogParameter(Parameter::Id::A2),
                         AnalogParameter(Parameter::Id::A3),
                         AnalogParameter(Parameter::Id::A4),
                         AnalogParameter(Parameter::Id::A5)},
      digital_parameters_{DigitalParameter(Parameter::Id::B1),
                          DigitalParameter(Parameter::Id::B2),
                          DigitalParameter(Parameter::Id::B3),
                          DigitalParameter(Parameter::Id::B4),
                          DigitalParameter(Parameter::Id::B5),
                          DigitalParameter(Parameter::Id::B6),
                          DigitalParameter(Parameter::Id::B7),
                          DigitalParameter(Parameter::Id::B8)} {}

AnalogParameter &TelemetryData::getAnalog(Parameter::Id id) {
  signal_easel_validate(Parameter::getType(id) == Parameter::Type::Analog);
  return analog_parameters_.at(static_cast<uint8_t>(id));
}

DigitalParameter &TelemetryData::getDigital(Parameter::Id id) {
  signal_easel_validate(Parameter::getType(id) == Parameter::Type::Digital);
  const size_t index =
      static_cast<uint8_t>(id) - static_cast<uint8_t>(Parameter::Id::B1);
  return digital_parameters_.at(index);
}

void TelemetryData::setTelemetryStationAddress(const std::string &address) {
  if (address.size() > TELEMETRY_STATION_ADDRESS_MAX_LENGTH_) {
    throw Exception(Exception::Id::APRS_INVALID_SOURCE_ADDRESS_LENGTH, address);
  }
  telemetry_station_address_ = address;
}

void TelemetryData::setTelemetryStationAddress(const std::string &address,
                                               uint8_t ssid) {
  if (address.size() > 6) {
    throw Exception(Exception::Id::APRS_INVALID_SOURCE_ADDRESS_LENGTH,
                    "w/o ssid: " + address);
  }

  if (ssid > 15) {
    throw Exception(Exception::Id::APRS_INVALID_SOURCE_ADDRESS_LENGTH,
                    "ssid" + std::to_string(ssid));
  }

  telemetry_station_address_ =
      address + "-" + std::to_string(static_cast<int>(ssid));
}

bool TelemetryData::setSequenceNumber(uint16_t sequence_number) {
  if (sequence_number > SEQUENCE_NUMBER_MAX_) {
    sequence_number_ = 0;
    return false;
  }
  sequence_number_ = sequence_number;
  return true;
}

std::string TelemetryData::getSequenceNumberString() const {
  std::string sequence_number;
  sequence_number += std::to_string(sequence_number_ / 100);
  sequence_number += std::to_string((sequence_number_ / 10) % 10);
  sequence_number += std::to_string(sequence_number_ % 10);
  return sequence_number;
};

bool TelemetryData::setComment(const std::string &comment) {
  if (comment.size() > COMMENT_MAX_LENGTH_) {
    comment_ = comment.substr(0, COMMENT_MAX_LENGTH_);
    return false;
  }
  comment_ = comment;
  return true;
}

bool TelemetryData::setProjectTitle(const std::string &project_title) {
  if (project_title.size() > PROJECT_TITLE_MAX_LENGTH_) {
    project_title_ = project_title.substr(0, PROJECT_TITLE_MAX_LENGTH_);
    return false;
  }
  project_title_ = project_title;
  return true;
}

} // namespace signal_easel::aprs::telemetry