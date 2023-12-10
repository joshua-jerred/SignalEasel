/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   ax25_address.cpp
 * @date   2023-12-09
 * @brief  The AX.25 address class implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/ax25.hpp>
#include <SignalEasel/exception.hpp>

namespace signal_easel::ax25 {

Address::Address(std::string address_string, uint8_t ssid,
                 bool is_last_address) {
  setAddressString(address_string);
  setSsid(ssid);
  setIsLastAddress(is_last_address);
}

Address::Address(AddressArray address_array) { decodeAddress(address_array); }

Address::AddressArray Address::encodeAddress() {
  assertIsValid();

  AddressArray address_array;

  size_t i = 0;
  for (; i < address_string_.length(); i++) {
    address_array.at(i) = address_string_.at(i) << 1;
  }
  for (; i < K_ADDRESS_CHARS_LENGTH; i++) {
    address_array.at(i) = ' ' << 1;
  }

  uint8_t ssid_byte = 0;
  if (command_or_response_) {
    ssid_byte |= 0b10000000;
  }
  if (reserve_bit_1_) {
    ssid_byte |= 0b01000000;
  }
  if (reserve_bit_2_) {
    ssid_byte |= 0b00100000;
  }
  if (is_last_address_) {
    ssid_byte |= 0b00000001;
  }
  ssid_byte |= ssid_ << 1;

  address_array.at(6) = ssid_byte;

  return address_array;
}

void Address::decodeAddress(Address::AddressArray address_array) {
  address_string_ = "";

  for (size_t i = 0; i < K_ADDRESS_CHARS_LENGTH; i++) {
    uint8_t address_char = address_array.at(i) >> 1;

    if (!std::isalnum(address_char) && address_char != ' ') {
      throw std::runtime_error("Invalid character in address " +
                               std::to_string(address_char));
      // throw Exception(Exception::Id::AX25_INVALID_CHARACTER_IN_ADDRESS);
    }

    // exit if we hit a space
    if (address_char == ' ') {
      break;
    }

    if (std::isalpha(address_char)) {
      address_char = std::toupper(address_char);
    }

    address_string_ += address_char;
  }

  assertAddressStringIsValid();

  uint8_t ssid_byte = address_array.at(6);
  command_or_response_ = (ssid_byte & 0b10000000) > 0;
  reserve_bit_1_ = (ssid_byte & 0b01000000) > 0;
  reserve_bit_2_ = (ssid_byte & 0b00100000) > 0;
  is_last_address_ = (ssid_byte & 0b00000001) > 0;
  ssid_ = (ssid_byte >> 1) & 0x0F;

  assertSsidIsValid();
}

bool Address::isSsidValid() { return ssid_ <= K_MAX_SSID_VALUE; }

bool Address::isAddressStringValid() {
  if (address_string_.length() > K_MAXIMUM_ADDRESS_LENGTH ||
      address_string_.length() < K_MINIMUM_ADDRESS_LENGTH) {
    return false;
  }

  for (uint8_t c : address_string_) {
    if (!std::isalnum(c)) {
      std::cout << "Invalid character: " << c << std::endl;
      return false;
    }
  }

  return true;
}

void Address::assertAddressStringIsValid() {
  if (address_string_.length() > K_MAXIMUM_ADDRESS_LENGTH ||
      address_string_.length() < K_MINIMUM_ADDRESS_LENGTH) {
    throw Exception(Exception::Id::AX25_INVALID_ADDRESS_LENGTH);
  }

  for (uint8_t c : address_string_) {
    if (!std::isalnum(c) && c != ' ') {
      throw Exception(Exception::Id::AX25_INVALID_CHARACTER_IN_ADDRESS);
    }
  }
}

void Address::assertSsidIsValid() {
  if (isSsidValid()) {
    return;
  }
  throw Exception(Exception::Id::AX25_INVALID_SSID);
}

void Address::setAddressString(std::string address_string) {
  address_string_ = address_string;
  assertAddressStringIsValid();
}

void Address::setSsid(uint8_t ssid) {
  ssid_ = ssid;
  assertSsidIsValid();
}

bool Address::isValid() { return isAddressStringValid() && isSsidValid(); }

std::ostream &operator<<(std::ostream &os, const Address &frame) {
  os << frame.address_string_ << "-";
  os << std::dec << (int)frame.ssid_;
  return os;
}

} // namespace signal_easel::ax25