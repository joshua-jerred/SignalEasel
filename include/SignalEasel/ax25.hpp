/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   ax25.hpp
 * @date   2023-02-11
 * @brief  AX.25 tools
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef AX25_HPP_
#define AX25_HPP_

#include <cstdint>

#include <array>
#include <iostream>
#include <string>
#include <vector>

#include <SignalEasel/bit_stream.hpp>

// #include "wavgen.h"

namespace signal_easel::ax25 {

inline constexpr size_t K_MINIMUM_ADDRESS_LENGTH = 3;
inline constexpr size_t K_MAXIMUM_ADDRESS_LENGTH = 6;
inline constexpr size_t K_MAX_SSID_VALUE = 15;
inline constexpr size_t K_ADDRESS_CHARS_LENGTH = 6;

inline constexpr size_t K_MAX_REPEATER_ADDRESSES = 2;
inline constexpr size_t K_MAX_INFORMATION_LENGTH = 256;

inline constexpr uint8_t K_FLAG = 0x7E;
inline constexpr uint8_t K_PID = 0xF0;

inline constexpr size_t K_PREAMBLE_LENGTH = 33;
inline constexpr size_t K_POSTAMBLE_LENGTH = 4;

/**
 * @brief Calculate the Frame Check Sequence (FCS) on a vector of bytes
 * @details From: https://gist.github.com/andresv/4611897
 * @param input_data The data to calculate the FCS on
 * @return uint16_t The calculated FCS
 */
uint16_t calculateFcs(const std::vector<uint8_t> &input_data);

/**
 * @brief AX.25 Address class. For encoding and decoding AX.25 addresses.
 * @details See AX.25 2.2 3.12.2 and 3.12.3
 */
class Address {
public:
  typedef std::array<uint8_t, 7> AddressArray;

  Address() = default;
  Address(std::string address_string, uint8_t ssid,
          bool is_last_address = false);
  Address(AddressArray address_array);

  /**
   * @brief Set the Address String object
   * @param address_string The address string to set
   * @return true if the address string was successfully set
   */
  void setAddressString(std::string address_string);
  std::string getAddressString() const { return address_string_; }

  /**
   * @brief Set the SSID (0-15)
   * @param ssid The 4 bit SSID to set
   * @return true if the SSID was successfully set/is valid
   */
  void setSsid(uint8_t ssid);
  uint8_t getSsid() const { return ssid_; }

  /**
   * @brief Set the as the last address in the AX.25 frame (Sets a bit in the
   * ssid byte)
   */
  void setIsLastAddress(bool is_last_address) {
    is_last_address_ = is_last_address;
  }
  bool isLastAddress() const { return is_last_address_; }

  /**
   * @brief Also used as the "Has been repeated" bit
   *
   * @return true
   * @return false
   */
  bool isCommandOrResponse() const { return command_or_response_; }
  bool isReservedBit1Set() const { return reserve_bit_1_; }
  bool isReservedBit2Set() const { return reserve_bit_2_; }

  /**
   * @brief Decode an AX.25 address from it's byte array representation
   * @param address The address to decode
   * @exception signal_easel::Exception Thrown if the address/SSID is invalid
   */
  void decodeAddress(AddressArray address);

  /**
   * @brief Get the address as a byte array
   * @return AddressArray
   */
  AddressArray encodeAddress();

  /**
   * @brief If the address is valid (can be encoded correctly or was decoded)
   * @return true if the address is valid false otherwise
   */
  bool isValid();

  /**
   * @brief Checks if the address is valid, if not throws an exception with
   * a specific error code.
   */
  void assertIsValid() {
    assertAddressStringIsValid();
    assertSsidIsValid();
  }

  friend std::ostream &operator<<(std::ostream &os, const Address &address);

  bool isSsidValid();
  bool isAddressStringValid();

private:
  void assertAddressStringIsValid();
  void assertSsidIsValid();

  std::string address_string_ = "";
  uint8_t ssid_ = 0x00;
  bool command_or_response_ = true;
  bool reserve_bit_1_ = true;
  bool reserve_bit_2_ = true;
  bool is_last_address_ = false;
};

class Frame {
public:
  /**
   * @brief The frame type of the AX-25 frame
   * @details There are three main frame types: Information, Supervisory, and
   * Unnumbered.
   * Currently only Unnumbered Information frames are supported.
   */
  enum class Type : uint8_t { // Currently only UI is supported
    UI = 0x03                 // Unnumbered Information
  };

  Frame() = default;

  /**
   * @brief setters/getters for the frame.
   * @exception signal_easel::Exception Thrown with specific error code upon
   * invalid input.
   * @{
   */
  void setDestinationAddress(Address address);
  Address getDestinationAddress() const { return destination_address_; }
  void setSourceAddress(Address address);
  Address getSourceAddress() const { return source_address_; }
  void addRepeaterAddress(Address address);
  std::vector<Address> getRepeaterAddresses() const {
    return repeater_addresses_;
  }
  void setInformation(std::vector<uint8_t> information);
  std::vector<uint8_t> getInformation() const { return information_; }
  /**
   * @}
   */

  bool isFrameValid();

  /**
   * @brief Builds all internal portions of the frame. Doesn't include
   * the flags.
   * @return std::vector<uint8_t> The built frame
   */
  std::vector<uint8_t> buildFrame();

  /**
   * @brief A step beyond buildFrame. This function builds the frame, does the
   * bit stuffing, adds the flags, and NRZI encodes the frame.
   * @return std::vector<uint8_t>
   */
  std::vector<uint8_t> encodeFrame();

  /**
   * @brief Attempt to parse an incoming bit stream into an AX.25 frame
   * @param bit_stream The bit stream to parse (NRZI encoded)
   * @return true if the frame was successfully parsed
   * @return false if the frame was not successfully parsed
   */
  bool parseBitStream(BitStream &bit_stream);

  friend std::ostream &operator<<(std::ostream &os, const Frame &address);

private:
  void addToBuildBuffer(uint8_t byte, bool reverse = true);
  uint16_t calculateFcsOnBuildBuffer();
  void AddByteForFcs(uint8_t byte);

  Address destination_address_ = Address();
  Address source_address_ = Address();
  std::vector<Address> repeater_addresses_ = {};
  Type type_ = Type::UI;
  uint8_t pid_ = K_PID;
  std::vector<uint8_t> information_ = {};
  uint16_t fcs_ = 0xFFFF;

  std::vector<uint8_t> build_buffer_ = {};
};

std::ostream &operator<<(std::ostream &os, const Address &frame);

} // namespace signal_easel::ax25

// Debugging

#endif // AX25_H_