/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   ax25_frame.cpp
 * @date   2023-12-09
 * @brief  General AX.25 frame class
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <type_traits>

#include "ax25.hpp"

namespace signal_easel::ax25 {

/**
 * @brief Reverse a byte
 * @param byte The byte to reverse
 * @return uint8_t The reversed byte
 */
uint8_t reverseByte(uint8_t byte) {
  static uint8_t nibble_flip[16] = {
      0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
      0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF,
  };
  return (nibble_flip[byte & 0xF] << 4) | nibble_flip[byte >> 4];
}

void Frame::setDestinationAddress(Address address) {
  address.assertIsValid();
  destination_address_ = address;
}

void Frame::setSourceAddress(Address address) {
  address.assertIsValid();
  source_address_ = address;
}

void Frame::addRepeaterAddress(Address address) {
  address.assertIsValid();

  if (repeater_addresses_.size() >= K_MAX_REPEATER_ADDRESSES) {
    throw std::runtime_error("Too many repeater addresses");
  }

  repeater_addresses_.push_back(address);
}

void Frame::setInformation(std::vector<uint8_t> information) {
  if (information.size() > K_MAX_INFORMATION_LENGTH) {
    throw std::runtime_error("Information too long");
  }

  information_ = information;
}

bool Frame::isFrameValid() {
  if (!destination_address_.isValid() || !source_address_.isValid()) {
    return false;
  }

  if (repeater_addresses_.size() > K_MAX_REPEATER_ADDRESSES) {
    return false;
  }

  for (auto address : repeater_addresses_) {
    if (!address.isValid()) {
      return false;
    }
  }

  if (information_.size() > K_MAX_INFORMATION_LENGTH) {
    return false;
  }

  return true;
}

std::vector<uint8_t> Frame::buildFrame() {
  if (!isFrameValid()) {
    throw std::runtime_error("Frame is not valid");
  }

  build_buffer_.clear();

  // Add the destination address first
  destination_address_.setIsLastAddress(false);
  for (uint8_t byte : destination_address_.encodeAddress()) {
    build_buffer_.push_back(byte);
  }

  if (repeater_addresses_.size() < 0) {
    source_address_.setIsLastAddress(true);
  }

  // Add the source address
  for (uint8_t byte : source_address_.encodeAddress()) {
    build_buffer_.push_back(byte);
  }

  // Add the repeater addresses if there are any
  for (auto address : repeater_addresses_) {
    for (uint8_t byte : address.encodeAddress()) {
      build_buffer_.push_back(byte);
    }
  }

  // Add the control byte
  build_buffer_.push_back(
      static_cast<std::underlying_type_t<Frame::Type>>(type_));

  // Add the protocol ID
  build_buffer_.push_back(pid_);

  // Add the information
  for (uint8_t byte : information_) {
    build_buffer_.push_back(byte);
  }

  // Calculate the FCS
  uint16_t fcs_temp = calculateFcs(build_buffer_);
  fcs_ = 0;
  fcs_ |= (fcs_temp & 0xFF) << 8;
  fcs_ |= (fcs_temp & 0xFF00) >> 8;
  build_buffer_.push_back(fcs_ >> 8);
  build_buffer_.push_back(fcs_ & 0xFF);

  // std::cout << "FCS: " << std::hex << fcs_ << std::endl;

  return build_buffer_;
}

std::vector<uint8_t> Frame::encodeFrame() {
  BitStream bit_stream = BitStream();

  // Add the flag(s)
  for (size_t i = 0; i < K_PREAMBLE_LENGTH; i++) {
    uint8_t byte[1] = {K_FLAG};
    bit_stream.addBits(byte, 8);
  }

  // Start Bit Stuffing
  int consecutive_ones = 0;

  // Add the contents of the frame
  // first, reverse the bytes
  for (uint8_t byte : buildFrame()) {
    uint8_t reversed_byte = reverseByte(byte);
    for (int i = 7; i >= 0; i--) {
      bool bit = (reversed_byte >> i) & 1;
      if (bit) {
        consecutive_ones++;
        bit_stream.addOneBit();
      } else {
        consecutive_ones = 0;
        bit_stream.addZeroBit();
      }

      if (consecutive_ones == 5) {
        bit_stream.addZeroBit();
        consecutive_ones = 0;
      }
    }
  }

  // Add the FCS
  // for (int i = 15; i >= 0; i--) {
  //   bool bit = (fcs_ >> i) & 1;
  //   if (bit) {
  //     consecutive_ones++;
  //     bit_stream.addOneBit();
  //   } else {
  //     consecutive_ones = 0;
  //     bit_stream.addZeroBit();
  //   }

  //   if (consecutive_ones == 5) {
  //     bit_stream.addZeroBit();
  //     consecutive_ones = 0;
  //   }
  // }

  // Add the flag(s)
  for (size_t i = 0; i < K_POSTAMBLE_LENGTH; i++) {
    uint8_t byte[1] = {K_FLAG};
    bit_stream.addBits(byte, 8);
  }

  // NRZI encode the bit stream into a vector of bytes
  std::vector<uint8_t> encoded_frame{};

  // Used for NRZI encoding. If we encounter a 0, we flip the bit.
  bool previous_bit = 0;
  size_t byte_index = 0;
  uint8_t byte_buffer = 0;

  while (bit_stream.getBitStreamLength() > 0) {
    int8_t bit = bit_stream.popNextBit();
    if (bit == -1) {
      break; // allow for the last byte to be incomplete
    }

    if (bit == 1) {
      byte_buffer |= previous_bit << (7 - byte_index);
    } else {
      byte_buffer |= (!previous_bit) << (7 - byte_index);
      previous_bit = !previous_bit;
    }

    byte_index++;
    if (byte_index == 8) {
      encoded_frame.push_back(byte_buffer);
      byte_buffer = 0;
      byte_index = 0;
    }
  }

  return encoded_frame;
}

void Frame::addToBuildBuffer(uint8_t byte, bool reverse) {
  if (reverse) {
    byte = reverseByte(byte);
  }

  build_buffer_.push_back(byte);
}

std::ostream &operator<<(std::ostream &os, const Frame &frame) {
  os << frame.destination_address_ << ">" << frame.source_address_ << ":";
  for (uint8_t byte : frame.information_) {
    os << (char)byte;
  }
  os << " [fcs: 0x" << std::hex << frame.fcs_;
  os << "]";
  return os;
}

} // namespace signal_easel::ax25