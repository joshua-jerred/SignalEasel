/**
 * @file ax25.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief AX-25 Frame Encoding
 * @date 2023-02-11
 * @copyright Copyright (c) 2022
 * @version 0.1
 */

#ifndef AX25_H_
#define AX25_H_

#include <stdint.h>

#include <iostream>
#include <string>
#include <vector>

#include "bit-stream.h"
#include "wavgen.h"

namespace AX25 {

uint8_t reverse_bits(uint8_t byte);

/**
 * @brief The frame type of the AX-25 frame
 * @details There are three main frame types: Information, Supervisory, and
 * Unnumbered.
 *
 * Currently only Unnumbered Information frames are supported.
 */
enum class FrameType {  // Currently only UI is supported
  UI = 0x03             // Unnumbered Information
};

struct Address {
  Address(std::string address, uint8_t ssid_num, bool last_address = false);
  bool valid = false;
  uint8_t address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t ssid = 0x00;
};

class Frame {
 public:
  Frame();

  void AddAddress(Address address);
  void AddInformation(std::vector<uint8_t> information);
  void BuildFrame();

  void Print(bool as_hex = false);
  void PrintBitStream(bool as_hex = false);
  BitStream& GetBitStream();

 private:
  void AddByteToStream(uint8_t byte, bool reverse = true,
                       bool include_in_fcs = true, bool flag = false);
  void AddByteForFcs(uint8_t byte);

  bool frame_built_ = false;
  bool last_address_set_ = false;

  const uint8_t flag_ = 0x7E;
  std::vector<Address> addresses_ = {};
  const uint8_t control_ = 0x03;
  const uint8_t pid_ = 0xF0;
  std::vector<uint8_t> information_ = {};

  uint8_t fcs_lo_ = 0xFF;
  uint8_t fcs_hi_ = 0xFF;

  const int kPreambleLength_ = 33;
  const int kPostambleLength_ = 3;
  BitStream bit_stream_ = BitStream();

  bool previous_bit_ = 0;
  int consecutive_ones_ = 0;

  uint8_t full_frame_[256] = {0};
  int full_frame_length_ = 0;
};

class AX25Exception : public std::exception {
 public:
  AX25Exception(std::string message) : message_(message) {}

  const char* what() const throw() { return message_.c_str(); }

 private:
  std::string message_;
};

}  // namespace AX25

// Debugging

std::ostream& operator<<(std::ostream& os, const AX25::Address& frame);

#endif  // AX25_H_