/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file       telemetry_transcoders.hpp
 * @date       2024-08-17
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#pragma once

#include <vector>

#include <SignalEasel/aprs/telemetry_data.hpp>

namespace signal_easel::aprs::telemetry {

/// @brief A class for encoding and decoding APRS telemetry messages.
class TelemetryTranscoder {
public:
  /// @brief Encodes the parameter data into a data report message.
  /// @param[out] output - The encoded message. This will be cleared before
  /// encoding.
  /// @return \c true if the message was successfully encoded, \c false
  /// otherwise.
  static bool encodeDataReportMessage(const TelemetryData &data,
                                      std::vector<uint8_t> &output);

  /// @brief Encodes the parameter coefficients into a message.
  /// @param[out] output - The encoded message. This will be cleared before
  /// encoding.
  /// @return \c true if the message was successfully encoded, \c false
  /// otherwise.
  static bool encodeParameterCoefficientMessage(const TelemetryData &data,
                                                std::vector<uint8_t> &output);

  /// @brief Encodes the parameter names into a message.
  /// @param[out] output - The encoded message. This will be cleared before
  /// encoding.
  /// @return \c true if the message was successfully encoded, \c false
  /// otherwise.
  static bool encodeParameterNameMessage(const TelemetryData &data,
                                         std::vector<uint8_t> &output);

  /// @brief Encodes the parameter units and labels into a message.
  /// @param[out] output - The encoded message. This will be cleared before
  /// encoding.
  /// @return \c true if the message was successfully encoded, \c false
  /// otherwise.
  static bool encodeUnitAndLabelMessage(const TelemetryData &data,
                                        std::vector<uint8_t> &output);

  /// @brief Encodes the bit sense and project name into a message.
  /// @param[out] output - The encoded message. This will be cleared before
  /// encoding.
  /// @return \c true if the message was successfully encoded, \c false
  /// otherwise.
  static bool encodeBitSenseMessage(const TelemetryData &data,
                                    std::vector<uint8_t> &output);

  /// @brief Takes in the information field of an APRS telemetry message and
  /// decodes it with the appropriate method.
  /// @param message - The information field of the APRS telemetry message.
  /// @return \c true if the message was successfully decoded, \c false
  /// otherwise.
  static bool decodeMessage(TelemetryData &data,
                            const std::vector<uint8_t> &message);

private:
  /// @brief Used in the encode methods to add the telemetry station address to
  /// the message in the format ":N0CALL-1 :".
  /// @param data - The input telemetry data, which contains the address.
  /// @param[out] output - The message to add the address to.
  static void addTelemetryStationAddress(const TelemetryData &data,
                                         std::vector<uint8_t> &output);

  /// @brief Validate the first few bytes of a message.
  /// @param message - The message to validate.
  /// @param header - The header to validate against, ex: "PARM."
  /// @return \c true if the header matches, \c false if not.
  static bool decodeValidateHeader(const std::vector<uint8_t> &message,
                                   const std::string &header);

  /// @brief Decode a data report message.
  /// @param message - The message to decode.
  /// @return \c true if the message was decoded, \c false if not.
  static bool decodeDataReportMessage(TelemetryData &data,
                                      const std::vector<uint8_t> &message);

  /// @brief Decode a name or unit/label message. They're the same format.
  /// @param message - The message to decode.
  /// @param name_or_unit - \c true if decoding a name, \c false if decoding a
  /// unit or label.
  /// @return \c true if the message was decoded, \c false if not.
  static bool decodeParameterDescriptor(TelemetryData &data,
                                        const std::vector<uint8_t> &message,
                                        const bool name_or_unit);

  /// @brief Decode the parameter coefficients message.
  /// @param message - The message to decode.
  /// @return \c true if the message was decoded, \c false if not.
  static bool
  decodeParameterCoefficientMessage(TelemetryData &data,
                                    const std::vector<uint8_t> &message);

  /// @brief Decode the bit sense / project name message.
  /// @param message - The message to decode.
  /// @return \c true if the message was decoded, \c false if not.
  static bool decodeBitSenseMessage(TelemetryData &data,
                                    const std::vector<uint8_t> &message);
};

} // namespace signal_easel::aprs::telemetry