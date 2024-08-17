/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   exception.hpp
 * @date   2023-12-10
 * @brief  SignalEasel exceptions
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_EXCEPTION_HPP_
#define SIGNAL_EASEL_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace signal_easel {

class Exception : public std::exception {
public:
  enum class Id {
    FILE_OPEN_ERROR,
    NO_DATA_TO_WRITE,
    INVALID_CALL_SIGN,
    INVALID_CALL_SIGN_MODE,
    INVALID_MORSE_CHAR,
    EMPTY_INPUT_STRING,
    NON_MATCHING_FILTER_VECTORS,
    AX25_INVALID_ADDRESS_LENGTH,
    AX25_INVALID_SSID,
    AX25_INVALID_CHARACTER_IN_ADDRESS,
    AX25_FRAME_LAST_ADDRESS_ALREADY_SET,
    AX25_FRAME_TOO_MANY_ADDRESSES,
    AX25_FRAME_NEED_AT_LEAST_TWO_ADDRESSES,
    AX25_FRAME_ALREADY_BUILT,
    AX25_FRAME_NOT_BUILT,
    BASE_91_ENCODE_VALUE_NOT_CORRECT_SIZE,
    APRS_INVALID_SOURCE_ADDRESS_LENGTH,
    APRS_INVALID_SOURCE_SSID,
    APRS_INVALID_DESTINATION_ADDRESS_LENGTH,
    APRS_INVALID_DESTINATION_SSID,
    APRS_INVALID_SYMBOL,
    APRS_LOCATION_INVALID_TIME_CODE_LENGTH,
    APRS_INVALID_LOCATION_DATA,
    APRS_INVALID_MESSAGE_ADDRESSEE_LENGTH,
    PULSE_OPEN_ERROR,
    PULSE_WRITE_ERROR,
    PULSE_DRAIN_ERROR,
    PULSE_CLOSE_ERROR,
    PULSE_READ_ERROR,
    PULSE_AUDIO_DISABLED,
    APRS_RECEIVER_BUFFER_FULL,
    APRS_MESSAGE_NOT_IMPLEMENTED,
    APRS_INVALID_COMMENT_LENGTH,
    SSTV_MODE_NOT_IMPLEMENTED,
    SSTV_IMAGE_TOOLS_ERROR,
    SSTV_IMAGE_SIZE_MISMATCH,
    SSTV_INVALID_RGB_VALUE,
    SSTV_SCAN_LINE_BUFFER_SIZE_MISMATCH,
    SSTV_GET_PIXEL_FAILED,
    SSTV_COLOR_FORMAT_NOT_IMPLEMENTED,
    SSTV_INVALID_COLOR_TYPE,
    VALIDATION_ERROR,
    GENERIC,
    APRS_TELEMETRY
  };

  static std::string idToString(Id exception_id) {
    switch (exception_id) {
    case Id::FILE_OPEN_ERROR:
      return "File open error";
    case Id::NO_DATA_TO_WRITE:
      return "No data to write";
    case Id::INVALID_CALL_SIGN:
      return "Invalid call sign";
    case Id::INVALID_CALL_SIGN_MODE:
      return "Invalid call sign mode";
    case Id::INVALID_MORSE_CHAR:
      return "Invalid morse character";
    case Id::EMPTY_INPUT_STRING:
      return "Empty input string";
    case Id::NON_MATCHING_FILTER_VECTORS:
      return "Non matching filter vectors";
    case Id::AX25_INVALID_ADDRESS_LENGTH:
      return "Invalid AX25 address length";
    case Id::AX25_INVALID_SSID:
      return "Invalid AX25 SSID";
    case Id::AX25_INVALID_CHARACTER_IN_ADDRESS:
      return "Invalid character in AX25 address";
    case Id::AX25_FRAME_LAST_ADDRESS_ALREADY_SET:
      return "AX25 frame last address already set";
    case Id::AX25_FRAME_TOO_MANY_ADDRESSES:
      return "AX25 frame too many addresses";
    case Id::AX25_FRAME_NEED_AT_LEAST_TWO_ADDRESSES:
      return "AX25 frame needs at least two addresses";
    case Id::AX25_FRAME_ALREADY_BUILT:
      return "AX25 frame already built";
    case Id::AX25_FRAME_NOT_BUILT:
      return "AX25 frame not built";
    case Id::BASE_91_ENCODE_VALUE_NOT_CORRECT_SIZE:
      return "Base 91 encode value not correct size";
    case Id::APRS_INVALID_SOURCE_ADDRESS_LENGTH:
      return "Invalid APRS source address length";
    case Id::APRS_INVALID_SOURCE_SSID:
      return "Invalid APRS source SSID";
    case Id::APRS_INVALID_DESTINATION_ADDRESS_LENGTH:
      return "Invalid APRS destination address length";
    case Id::APRS_INVALID_DESTINATION_SSID:
      return "Invalid APRS destination SSID";
    case Id::APRS_INVALID_SYMBOL:
      return "Invalid APRS symbol";
    case Id::APRS_LOCATION_INVALID_TIME_CODE_LENGTH:
      return "Invalid APRS location time code length";
    case Id::APRS_INVALID_LOCATION_DATA:
      return "Invalid APRS location data";
    case Id::APRS_INVALID_MESSAGE_ADDRESSEE_LENGTH:
      return "Invalid APRS message addressee length";
    case Id::PULSE_OPEN_ERROR:
      return "PulseAudio open error";
    case Id::PULSE_WRITE_ERROR:
      return "PulseAudio write error";
    case Id::PULSE_DRAIN_ERROR:
      return "PulseAudio drain error";
    case Id::PULSE_CLOSE_ERROR:
      return "PulseAudio close error";
    case Id::PULSE_READ_ERROR:
      return "PulseAudio read error";
    case Id::PULSE_AUDIO_DISABLED:
      return "PulseAudio disabled";
    case Id::APRS_RECEIVER_BUFFER_FULL:
      return "APRS receiver buffer full";
    case Id::APRS_MESSAGE_NOT_IMPLEMENTED:
      return "APRS message not implemented";
    case Id::APRS_INVALID_COMMENT_LENGTH:
      return "Invalid APRS comment length";
    case Id::SSTV_MODE_NOT_IMPLEMENTED:
      return "SSTV mode not implemented";
    case Id::SSTV_IMAGE_TOOLS_ERROR:
      return "SSTV image tools error";
    case Id::SSTV_IMAGE_SIZE_MISMATCH:
      return "SSTV image size mismatch";
    case Id::SSTV_INVALID_RGB_VALUE:
      return "SSTV invalid RGB value";
    case Id::SSTV_SCAN_LINE_BUFFER_SIZE_MISMATCH:
      return "SSTV scan line buffer size mismatch";
    case Id::SSTV_GET_PIXEL_FAILED:
      return "SSTV get pixel failed";
    case Id::SSTV_COLOR_FORMAT_NOT_IMPLEMENTED:
      return "SSTV color format not implemented";
    case Id::SSTV_INVALID_COLOR_TYPE:
      return "SSTV invalid color type";
    case Id::VALIDATION_ERROR:
      return "Validation error";
    case Id::GENERIC:
      return "Generic";
    case Id::APRS_TELEMETRY:
      return "APRS telemetry";
    default:
      return "Unknown error";
    }
  };

  Exception(const Exception::Id &exception_id, const std::string &message = "")
      : id_(exception_id),
        exception_string_(idToString(exception_id) +
                          (message.size() > 0 ? " : " + message : "")) {}
  Exception(const std::string &message = "")
      : id_(Exception::Id::GENERIC),
        exception_string_(idToString(Exception::Id::GENERIC) +
                          (message.size() > 0 ? " : " + message : "")) {}
  virtual const char *what() const throw() { return exception_string_.c_str(); }

private:
  Exception::Id id_;
  std::string exception_string_;
};

inline void validate(bool condition, std::string message = "") {
  if (!condition) {
    throw Exception(Exception::Id::VALIDATION_ERROR, message);
  }
}

inline void validateWithContext(bool condition, const char *file, size_t line) {
  if (!condition) {
    throw Exception(Exception::Id::VALIDATION_ERROR,
                    std::string(file) + ":" + std::to_string(line));
  }
}

#define signal_easel_validate(__e)                                             \
  validateWithContext((__e), __FILE__, __LINE__)

} // namespace signal_easel

#endif /* SIGNAL_EASEL_EXCEPTION_HPP_ */