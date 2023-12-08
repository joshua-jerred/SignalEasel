/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   ax25.h
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

#include <iostream>
#include <string>
#include <vector>

#include <SignalEasel/signal_easel.hpp>

// #include "wavgen.h"

namespace signal_easel {

class Ax25Frame {
public:
  struct Address {
    Address(std::string address, uint8_t ssid_num, bool last_address = false);
    bool valid = false;
    uint8_t address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t ssid = 0x00;
  };

  /**
   * @brief The frame type of the AX-25 frame
   * @details There are three main frame types: Information, Supervisory, and
   * Unnumbered.
   * Currently only Unnumbered Information frames are supported.
   */
  enum class Type { // Currently only UI is supported
    UI = 0x03       // Unnumbered Information
  };

  Ax25Frame() = default;

  /**
   * @brief Add an address to the AX.25 frame. The first address added will be
   * the source address.
   * @param address The address to add
   */
  void AddAddress(Address address);
  void AddInformation(std::vector<uint8_t> information);
  std::vector<uint8_t> BuildFrame();

  void Print(bool as_hex = false);
  void PrintBitStream(bool as_hex = false);
  BitStream &GetBitStream();

  /**
   * @brief Attempt to parse an incoming bit stream into an AX.25 frame
   * @param bit_stream The bit stream to parse (NRZI encoded)
   * @return true if the frame was successfully parsed
   * @return false if the frame was not successfully parsed
   */
  bool parseBitStream(BitStream &bit_stream);

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

std::ostream &operator<<(std::ostream &os, const Ax25Frame::Address &frame);

} // namespace signal_easel

// Debugging

#endif // AX25_H_