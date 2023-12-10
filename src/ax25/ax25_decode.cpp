/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   ax25_decode.cpp
 * @date   2023-12-09
 * @brief  AX.25 decoding implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <bitset>
#include <iomanip>
#include <iostream>

#include <SignalEasel/ax25.hpp>

namespace signal_easel::ax25 {

const uint8_t AX25_FLAG = 0x7E;

uint8_t reverse_bits(uint8_t byte) {
  static uint8_t nibble_flip[16] = {
      0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
      0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF,
  };
  return (nibble_flip[byte & 0xF] << 4) | nibble_flip[byte >> 4];
}

void print_hex(uint8_t byte) {
  std::cout << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(byte);
}

/**
 * @brief Convert a bit stream from NRZI to standard encoding
 *
 * @param bit_stream The bit stream to decode (NRZI encoded)
 * @return BitStream The decoded bit stream
 */
BitStream decodeNrzi(BitStream &bit_stream) {
  BitStream nrzi_bit_stream;
  int previous_bit = 0;
  size_t num_bits = bit_stream.getBitStreamLength();
  while (num_bits > 0) {
    int bit = bit_stream.popNextBit();
    if (bit == -1) {
      std::cout << "No more bits in bit stream" << std::endl;
      break;
    }
    if (bit == previous_bit) {
      nrzi_bit_stream.addOneBit();
    } else {
      nrzi_bit_stream.addZeroBit();
    }
    previous_bit = bit;
    num_bits--;
  }
  nrzi_bit_stream.pushBufferToBitStream();
  return nrzi_bit_stream;
}

/**
 * @brief Finds the start flags in the bit stream and removes them.
 *
 * @param bit_stream The bit stream to search
 * @return int -1 if no flags were found, otherwise the number of opening flags
 */
int findStartFlags(BitStream &bit_stream) {
  size_t num_bits = bit_stream.getBitStreamLength();
  const auto &bit_vector = bit_stream.getBitVector();
  uint8_t byte_buffer = 0;
  int32_t bit_offset = -1;
  int word_offset = 0;
  for (uint32_t word : bit_vector) {
    for (int32_t i = 0; i < 32; i++) {
      byte_buffer = (word >> (24 - i)) & 0xFF;
      if (byte_buffer == AX25_FLAG) {
        bit_offset = i;
        break;
      }
    }
    if (bit_offset != -1) {
      break;
    }
    word_offset++;
  }

  if (bit_offset < 0) {
    return -1;
  }

  bit_offset = 32 * word_offset + bit_offset;

  while (bit_offset > 0) { // burn off the bits before the first flag
    bit_stream.popNextBit();
    bit_offset--;
  }

  num_bits = bit_stream.getBitStreamLength();
  byte_buffer = 0;
  int num_flags = 0;
  while (num_bits > 8) { // burn off the flags at the start of the frame
    byte_buffer = bit_stream.peakNextByte();

    if (byte_buffer == AX25_FLAG) {
      num_flags++;
      for (int i = 0; i < 8; i++) {
        bit_stream.popNextBit();
      }
    } else {
      break;
    }
    num_bits -= 8;
  }

  return num_flags;
}

std::vector<uint8_t> deStuffBytes(BitStream &bit_stream) {
  std::vector<uint8_t> destuffed_bytes;
  int consecutive_ones = 0;
  int num_bits = bit_stream.getBitStreamLength();
  uint8_t byte_buffer = 0;
  while (num_bits > 0) {
    byte_buffer = 0;
    for (int8_t i = 0; i < 8; i++) {
      if (consecutive_ones == 5) {
        if (bit_stream.peakNextBit() == 0) {
          bit_stream.popNextBit();
          num_bits--;
          consecutive_ones = 0;
          // std::cout << "Found stuffed bit" << std::endl;
        } else {
          // std::cout << "Found end flag" << std::endl;
          return destuffed_bytes;
        }
      }

      int8_t next_bit = bit_stream.popNextBit();
      num_bits--;
      if (next_bit == -1) {
        // std::cout << "Ran out of bits" << std::endl;
        // return false;
        num_bits = 0;
        return destuffed_bytes;
      }
      if (next_bit == 1) {
        consecutive_ones++;
      } else {
        consecutive_ones = 0;
      }
      byte_buffer = byte_buffer << 1;
      byte_buffer |= next_bit == 1 ? 1 : 0;
    }
    destuffed_bytes.push_back(reverse_bits(byte_buffer));
  }
  return destuffed_bytes;
}

bool Frame::parseBitStream(BitStream &bit_stream) {
  constexpr int k_min_start_flags = 2;
  constexpr int k_min_bytes = 20;

  BitStream nrzi_bit_stream = decodeNrzi(bit_stream);
  // nrzi_bit_stream.dumpBitStream();
  auto start_flags = findStartFlags(nrzi_bit_stream);
  if (start_flags < k_min_start_flags) {
    // std::cout << "Not enough start flags: " << start_flags << std::endl;
    return false;
  }

  std::vector<uint8_t> destuffed_bytes = deStuffBytes(nrzi_bit_stream);

  if (destuffed_bytes.size() < k_min_bytes) {
    // std::cout << "Not enough bytes" << std::endl;
    return false;
  }

  size_t iterator = 0;

  // parse the destination address
  std::string dest_address_string = "";
  for (; iterator < 6; iterator++) {
    char new_char = (char)(destuffed_bytes.at(iterator) >> 1);
    if (new_char == ' ') {
      continue;
    }
    dest_address_string += new_char;
  }
  uint8_t dest_ssid = (destuffed_bytes.at(iterator++) >> 1) & 0x0F;
  Address dest_address(dest_address_string, dest_ssid, false);
  setDestinationAddress(dest_address);

  // parse the source address
  std::string source_address_string = "";
  bool last_address = false;
  int num_sources = 0;
  while (!last_address) {
    source_address_string = "";
    size_t start = iterator;
    for (; iterator < start + 6; iterator++) {
      char new_char = (char)(destuffed_bytes.at(iterator) >> 1);
      if (new_char == ' ') {
        continue;
      }
      source_address_string += new_char;
    }
    uint8_t ssid_byte = destuffed_bytes.at(iterator++);
    last_address = ssid_byte & 0x01;
    uint8_t source_ssid = (ssid_byte >> 1) & 0x0F;
    Address source_address(source_address_string, source_ssid, last_address);
    if (num_sources == 0) {
      setSourceAddress(source_address);
    } else {
      addRepeaterAddress(source_address);
    }
  }

  // parse the control byte
  uint8_t control_byte = destuffed_bytes.at(iterator++);
  if (control_byte != 0x03) {
    std::cout << "Control byte is not 0x03" << std::endl;
    return false;
  }

  // parse the PID byte
  uint8_t pid_byte = destuffed_bytes.at(iterator++);
  if (pid_byte != 0xF0) {
    std::cout << "PID byte is not 0xF0" << std::endl;
    return false;
  }

  // parse the information
  const size_t k_fcs_length = 2;
  for (; iterator < destuffed_bytes.size() - k_fcs_length;) {
    this->information_.push_back(destuffed_bytes.at(iterator++));
  }

  // parse the FCS
  fcs_ = 0;
  fcs_ |= destuffed_bytes.at(iterator++) << 8;
  fcs_ |= destuffed_bytes.at(iterator++) & 0xFF;

  if (iterator != destuffed_bytes.size()) {
    std::cout << "Didn't parse all bytes" << std::endl;
    return false;
  }

  return true;
}

} // namespace signal_easel::ax25