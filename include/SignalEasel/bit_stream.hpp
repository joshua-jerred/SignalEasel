/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   bit_stream.hpp
 * @date   2023-12-10
 * @brief  The bit stream class
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_BIT_STREAM_HPP_
#define SIGNAL_EASEL_BIT_STREAM_HPP_

#include <cstdint>
#include <vector>

namespace signal_easel {

/**
 * @details Bit stream is an array of 32 bit integers.
 * The bits are stored in a 32 bit integer in the following way:
 *
 * Characters:
 * a a b b c c
 *
 * varicode with 2x 0 bit padding added to the end of each character:
 * '1011' '00' '1011' '00'  '1011111' '00' '1011111' '00' '101111' '00' '101111'
 * '00'
 *
 * 32 bit array:
 * [0](10110010110010111110010111110010) {a, a, b, b, half of c    }
 * [1](11110010111100000000000000000000) {other half of c, 0 bits..}
 *
 * Although this may not be the best way to store the bit stream,
 * it makes it easier to understand.
 * @todo Add support for little endian and big endian.
 */
class BitStream {
public:
  void dumpBitStream();
  void dumpBitStreamAsHex();
  void dumpBitStreamAsAscii();
  void addBits(const unsigned char *data, int num_bits);
  int8_t popNextBit();
  int peakNextBit();
  int peakNextByte();
  void pushBufferToBitStream();
  int getBitStreamLength() const; // Number of bits in the bit stream

  void addOneBit() { addBits((const unsigned char *)"\xff", 1); }
  void addZeroBit() { addBits((const unsigned char *)"\x00", 1); }

  const std::vector<uint32_t> &getBitVector() const { return bit_stream_; }

private:
  std::vector<uint32_t> bit_stream_ = std::vector<uint32_t>();
  int bit_stream_index_ = 0;
  uint32_t bit_stream_buffer_ = 0;
  int bit_stream_offset_ = 0; // Write left to right. [0 - 31]
  int bit_stream_length_ = 0; // Number of bits in the bit stream
};

} // namespace signal_easel

#endif /* SIGNAL_EASEL_BIT_STREAM_HPP_ */