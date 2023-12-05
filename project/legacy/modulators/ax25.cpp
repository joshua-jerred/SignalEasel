#include "ax25.h"

#include <bitset>
#include <iomanip>
#include <iostream>

#include "modulators.h"

uint8_t AX25::reverse_bits(uint8_t byte) {
  static uint8_t nibble_flip[16] = {
      0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
      0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF,
  };
  return (nibble_flip[byte & 0xF] << 4) | nibble_flip[byte >> 4];
}

AX25::Address::Address(std::string address, uint8_t ssid_num,
                       bool last_address) {
  if (address.length() < 2 || address.length() > 6) {
    throw AX25::AX25Exception("Invalid address length: " +
                              std::to_string(address.length()));
  }

  if (ssid_num > 15) {  // 4 bit value
    throw AX25::AX25Exception("Invalid SSID number: " +
                              std::to_string(ssid_num));
  }

  for (unsigned int i = 0; i < address.length(); i++) {
    if ((address[i] < 0x41 || address[i] > 0x5A) &&
        (address[i] < 0x30 || address[i] > 0x39)) {
      throw AX25::AX25Exception("Invalid character in address: " +
                                std::to_string(address[i]));
    }

    // In the address field, shift each character left by one bit
    this->address[i] = address[i] << 1;
  }

  // Add space characters to the end of the address
  for (unsigned int i = address.length(); i < 6; i++) {
    this->address[i] = 0x40;
  }

  // SSID bit pattern: 011 SSID x
  // CORRECTION TO ABOVE ^ it's 111 SSID x according to what direwolf does (i
  // need to read the spec more?) SSID is 4 bits, the ssid binary value x is 1
  // if it's the last address, 0 otherwise
  this->ssid = 0xe0 | (ssid_num << 1) | (last_address ? 0x01 : 0x00);
}

AX25::Frame::Frame() {}

void AX25::Frame::AddAddress(Address address) {
  if (last_address_set_) {
    throw AX25::AX25Exception("Last address already set");
  }

  if (addresses_.size() >= 7) {
    throw AX25::AX25Exception("Too many addresses");
  }

  addresses_.push_back(address);

  if (address.ssid & 0x01) {
    last_address_set_ = true;
  }
}

void AX25::Frame::AddInformation(std::vector<uint8_t> information) {
  this->information_ = information;
}

// https://gist.github.com/andresv/4611897
uint16_t ax25crc16(unsigned char *data_p, uint16_t length) {
    uint16_t crc = 0xFFFF;
    uint32_t data;
    uint16_t crc16_table[] = {
            0x0000, 0x1081, 0x2102, 0x3183,
            0x4204, 0x5285, 0x6306, 0x7387,
            0x8408, 0x9489, 0xa50a, 0xb58b,
            0xc60c, 0xd68d, 0xe70e, 0xf78f
    };

    while(length--){
        crc = ( crc >> 4 ) ^ crc16_table[(crc & 0xf) ^ (*data_p & 0xf)];
        crc = ( crc >> 4 ) ^ crc16_table[(crc & 0xf) ^ (*data_p++ >> 4)];
    }

    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xff); // do byte swap here that is needed by AX25 standard
    return (~crc);
}

void AX25::Frame::BuildFrame() {
  if (addresses_.size() < 2) {
    throw AX25::AX25Exception("Need at least 2 addresses");
  }
  if (frame_built_) {
    throw AX25::AX25Exception("Frame already built");
  }

  for (int i = 0; i < kPreambleLength_; i++) {
    AddByteToStream(flag_, false, false, true);
  }

  for (Address address : addresses_) {
    for (uint8_t byte : address.address) {
      AddByteToStream(byte);
    }
    AddByteToStream(address.ssid);
  }

  AddByteToStream(control_);
  AddByteToStream(pid_);

  for (uint8_t byte : information_) {
    AddByteToStream(byte);
  }

  uint16_t fcs = ax25crc16(full_frame_, full_frame_length_);
  uint8_t fcs_hi_ = fcs & 0xff;
  uint8_t fcs_lo_ = fcs >> 8;
  AddByteToStream(fcs_lo_, true, false); // FCS
  AddByteToStream(fcs_hi_, true, false);

  for (int i = 0; i < kPostambleLength_; i++) {
    AddByteToStream(flag_, false, false, true);
  }

  bit_stream_.pushBufferToBitStream();
  frame_built_ = true;
}

void AX25::Frame::AddByteToStream(uint8_t byte, bool reverse,
                                  bool include_in_fcs, bool flag) {
  if (include_in_fcs) {
    AddByteForFcs(byte);
  }
  uint8_t zero[1] = {0};
  uint8_t one[1] = {0xff};
  uint8_t rev_byte = 0;
  if (reverse) {
    rev_byte = reverse_bits(byte);
  } else {
    rev_byte = byte;
    //rev_byte = byte;
  }

  if (flag) {
    uint8_t flag_byte[1] = {rev_byte};
    bit_stream_.addBits(flag_byte, 8);
    return;
  }

  // Bit Stuffing
  for (int i = 0; i < 8; i++) {
    if (consecutive_ones_ == 5) {
      bit_stream_.addBits(zero, 1);
      consecutive_ones_ = 0;
      previous_bit_ = 0;
    }
    if ((rev_byte >> (7 - i)) & 1) {
      bit_stream_.addBits(one, 1);
      consecutive_ones_++;
      previous_bit_ = 1;
    } else {
      bit_stream_.addBits(zero, 1);
      consecutive_ones_ = 0;
      previous_bit_ = 0;
    }
  }
}

void AX25::Frame::AddByteForFcs(uint8_t byte) {
  full_frame_[full_frame_length_++] = byte;
}

void print_hex(uint8_t byte) {
  std::cout << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(byte);
}

void AX25::Frame::Print(bool as_hex) {
  if (!frame_built_) {
    throw AX25::AX25Exception("Frame not built");
  }
  if (addresses_.size() < 2) {
    throw AX25::AX25Exception("Need at least 2 addresses");
  }
  std::cout << addresses_[1] << "> " << addresses_[0];
  if (addresses_.size() > 2) {
    for (unsigned int i = 2; i < addresses_.size(); i++) {
      std::cout << "," << addresses_[i];
      if (addresses_[i].ssid & 0x01) {
        std::cout << ":";  // Last address
      }
    }
  }
  for (uint8_t byte : information_) {
    std::cout << (char)byte;
  }
  std::cout << std::endl;

  if (as_hex) {
    for (unsigned int i = 0; i < addresses_.size(); i++) {
      for (uint8_t byte : addresses_[i].address) {
        print_hex(byte);
        std::cout << " ";
      }
      print_hex(addresses_[i].ssid);
      std::cout << " ";
    }
    print_hex(control_);
    std::cout << " ";
    print_hex(pid_);
    std::cout << " ";
    for (uint8_t byte : information_) {
      print_hex(byte);
      std::cout << " ";
    }
  }

  std::cout << "FCS: ";
  print_hex(fcs_hi_);
  std::cout << " ";
  print_hex(fcs_lo_);
  std::cout << std::endl;
}

void AX25::Frame::PrintBitStream(bool as_hex) {
  if (!frame_built_) {
    throw AX25::AX25Exception("Frame not built");
  }
  
  if (as_hex) {
    bit_stream_.dumpBitStreamAsHex();
  } else {
    bit_stream_.dumpBitStream();
  }
}

BitStream& AX25::Frame::GetBitStream() {
  if (!frame_built_) {
    throw AX25::AX25Exception("Frame not built");
  }
  return bit_stream_;
}

std::ostream& operator<<(std::ostream& os, const AX25::Address& frame) {
  for (int i = 0; i < 6; i++) {
    if (frame.address[i] == 0x40) {
      std::cout << "";
    } else {
      std::cout << (char)(frame.address[i] >> 1);
    }
  }
  int ssid = (frame.ssid & 0b00011110) >> 1;
  std::cout << "-" << ssid;
  return os;
}