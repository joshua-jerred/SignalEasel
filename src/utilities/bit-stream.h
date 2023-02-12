/**
 * @file bit-stream.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief A contiguous stream of bits.
 * @date 2023-02-11
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#ifndef BIT_STREAM_H_
#define BIT_STREAM_H_

#include <vector>
#include <cstdint>

/**
 * @details Bit stream is an array of 32 bit integers.
 * The bits are stored in a 32 bit integer in the following way:
 * 
 * Characters: 
 * a a b b c c
 * 
 * varicode with 2x 0 bit padding added to the end of each character: 
 * '1011' '00' '1011' '00'  '1011111' '00' '1011111' '00' '101111' '00' '101111' '00'
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
    void addBits(unsigned char *data, int num_bits);
    int popNextBit();
    int peakNextBit();
    void pushBufferToBitStream();
    int getBitStreamLength();

  private:
    std::vector<uint32_t> bit_stream_ = std::vector<uint32_t>();
    int bit_stream_index_ = 0;
    uint32_t bit_stream_buffer_ = 0;
    int bit_stream_offset_ = 0; // Write left to right. [0 - 31]
    int bit_stream_length_ = 0;
};

#endif // BIT_STREAM_H_