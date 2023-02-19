#include "ax25.h"

#include <iomanip>

#include "bit-stream.h"
#include "modulators.h"

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

void AX25::Frame::addAddAddress(Address address) {
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

  /*
  std::cout << std::endl;

  for (int i = 0; i < 6; i++) {
    os << "0x";
    os << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
       << (int)frame.address[i] << " ";
  }

  os << "0x";
  os << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
     << (int)frame.ssid << std::endl;
  */
  return os;
}

void AX25::Frame::print() {
  if (addresses_.size() < 2) {
    throw AX25::AX25Exception("Need at least 2 addresses");
  }
  std::cout << addresses_[1] << ">" << addresses_[0];
  if (addresses_.size() > 2) {
    for (unsigned int i = 2; i < addresses_.size(); i++) {
      std::cout << "," << addresses_[i];
      if (addresses_[i].ssid & 0x01) {
        std::cout << ":";  // Last address
      }
    }
  }
  std::cout << std::endl;
}

int main() {
  AX25::Address address1("KK7EWJ", 0, false);
  AX25::Address address2("KD9GDC", 11, false);
  AX25::Address address3("WIDE2", 1, true);

  AX25::Frame frame;
  frame.addAddAddress(address1);
  frame.addAddAddress(address2);
  frame.addAddAddress(address3);
  frame.print();

  return 0;
}