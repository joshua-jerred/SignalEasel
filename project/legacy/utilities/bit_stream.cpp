#include <bitset>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#include "bit-stream.h"

void BitStream::addBits(unsigned char *data, int num_bits) {
  int data_index = 0;
  int bit_index = 0; // Index of the bit in the byte, left to right [0 - 7]
  int buffer_space;
  bit_stream_length_ += num_bits;
  while (num_bits > 0) {
    buffer_space = 32 - bit_stream_offset_;

    if (buffer_space == 0) { // buffer is full, write to bit stream
      // std::cout << "Buffer full, writing to bit stream" << std::endl;
      bit_stream_.push_back(bit_stream_buffer_);
      bit_stream_buffer_ = 0;
      bit_stream_offset_ = 0;
      buffer_space = 32;
    }

    if (buffer_space >= num_bits && num_bits > 8) { // Write a byte at a time
      // std::cout << "Writing a byte at a time" << std::endl;
      bit_stream_buffer_ |= (data[data_index] << (32 - bit_stream_offset_ - 8));
      bit_stream_offset_ += 8;
      num_bits -= 8;
      data_index++;
    } else { // Write a bit at a time
      bit_stream_buffer_ |= (data[data_index] & (1 << (7 - bit_index)))
                                ? 1 << (32 - bit_stream_offset_ - 1)
                                : 0;
      bit_stream_offset_++;
      num_bits--;
      bit_index++;
      if (bit_index == 8) {
        bit_index = 0;
        data_index++;
      }
    }
  }
}

/**
 * @brief After adding all of the data to the bit stream, this method will
 * write the data left in the buffer
 *
 */
void BitStream::pushBufferToBitStream() {
  bit_stream_.push_back(bit_stream_buffer_);
  bit_stream_buffer_ = 0;
  bit_stream_offset_ = 0;
  bit_stream_index_ = 0;
}

/**
 * @brief Returns the next bit of the bit stream that will be encoded.
 *
 * @return int [1, 0, -1] -1 = no more bits in bit stream
 */
int BitStream::popNextBit() {
  if (bit_stream_index_ >=
      (int)bit_stream_.size()) { // No more bits in bit stream
    return -1;
  }

  uint32_t bit =
      bit_stream_[bit_stream_index_] & (1 << (31 - bit_stream_offset_));

  bit_stream_offset_++;
  if (bit_stream_offset_ == 32) {
    bit_stream_offset_ = 0;
    bit_stream_index_++;
  }
  bit_stream_length_--;
  return bit ? 1 : 0;
}

/**
 * @brief Returns the next bit of the bit stream without removing it.
 * @return int [1, 0, -1] -1 = no more bits in bit stream
 */
int BitStream::peakNextBit() {
  if (bit_stream_index_ >=
      (int)bit_stream_.size()) { // No more bits in bit stream
    return -1;
  }
  uint32_t bit =
      bit_stream_[bit_stream_index_] & (1 << (31 - bit_stream_offset_));
  return bit ? 1 : 0;
}

void BitStream::dumpBitStream() {
  std::cout << "BitStream:" << std::endl;
  for (int i = 0; i < (int)bit_stream_.size(); i++) {
    std::cout << "[" << i << "]" << std::bitset<8>(bit_stream_[i] >> 24) << " "
              << std::bitset<8>(bit_stream_[i] >> 16) << " "
              << std::bitset<8>(bit_stream_[i] >> 8) << " "
              << std::bitset<8>(bit_stream_[i]) << std::endl;
  }
}

void BitStream::dumpBitStreamAsHex() {
  std::cout << "BitStream:" << std::endl;
  for (int i = 0; i < (int)bit_stream_.size(); i++) {
    std::cout << "[" << i << "]" << std::hex << std::setfill('0')
              << std::setw(2) << ((bit_stream_[i] >> 24) & 0xFF) << " "
              << std::hex << std::setfill('0') << std::setw(2)
              << ((bit_stream_[i] >> 16) & 0xFF) << " " << std::hex
              << std::setfill('0') << std::setw(2)
              << ((bit_stream_[i] >> 8) & 0xFF) << " " << std::hex
              << std::setfill('0') << std::setw(2) << ((bit_stream_[i]) & 0xFF)
              << std::endl;
  }
}

void BitStream::dumpBitStreamAsAscii() {
  std::cout << "BitStream:" << std::endl;
  for (int i = 0; i < (int)bit_stream_.size(); i++) {
    std::cout << "[" << i << "]"
              << static_cast<unsigned char>(((bit_stream_[i] >> 24) & 0xFF))
              << " "
              << static_cast<unsigned char>(((bit_stream_[i] >> 16) & 0xFF))
              << " "
              << static_cast<unsigned char>(((bit_stream_[i] >> 8) & 0xFF))
              << " " << static_cast<unsigned char>(((bit_stream_[i]) & 0xFF))
              << std::endl;
  }
}

int BitStream::getBitStreamLength() {
  return bit_stream_length_;
}