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
  // SSID is 4 bits, the ssid binary value
  // x is 1 if it's the last address, 0 otherwise
  this->ssid = 0x60 | (ssid_num << 1) | (last_address ? 0x01 : 0x00);
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

void AX25::Frame::BuildFrame() {
  if (addresses_.size() < 2) {
    throw AX25::AX25Exception("Need at least 2 addresses");
  }
  if (frame_built_) {
    throw AX25::AX25Exception("Frame already built");
  }

  for (int i = 0; i < kPreambleLength; i++) {
    AddByteToStream(flag_, false, false);
  }

  // Build the frame and calculate the FCS
  uint8_t fcshi = 0xFF;
  uint8_t fcslo = 0xFF;

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

  AddByteToStream(fcslo, true, false);
  AddByteToStream(fcshi, true, false);

  AddByteToStream(flag_, false, false);

  bit_stream_.pushBufferToBitStream();
  frame_built_ = true;
}

void AX25::Frame::AddByteToStream(uint8_t byte, bool reverse,
                                  bool include_in_fcs) {
  if (include_in_fcs) {
    AddByteForFcs(byte);
  }

  uint8_t rev_byte[1] = {0};
  if (reverse) {
    rev_byte[0] = reverse_bits(byte);
  } else {
    rev_byte[0] = byte;
  }
  bit_stream_.addBits(rev_byte, 8);
}

void AX25::Frame::AddByteForFcs(uint8_t byte) {
  (void)byte;
}

void AX25::Frame::Print() {
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
}

void AX25::Frame::PrintBitStream() {
  if (!frame_built_) {
    throw AX25::AX25Exception("Frame not built");
  }

  bit_stream_.dumpBitStream();
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