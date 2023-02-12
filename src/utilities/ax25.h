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

#include <vector>
#include <iostream>

struct AX25UiFrame {
  const uint8_t flag = 0x7E;
  uint8_t destination_address[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t source_address[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  std::vector<uint8_t> digipeater_addresses;
  const uint8_t control = 0x03;
  const uint8_t protocol_id = 0xF0;
  std::vector<uint8_t> information;
  uint8_t fcs[2] = {0x00, 0x00};
  const uint8_t flag2 = 0x7E;
};

// Print overloads
std::ostream& operator<<(std::ostream& os, const AX25UiFrame& frame) {
  os << "AX25 Frame: " << std::endl;
  os << "  Destination Address: ";
  for (auto byte : frame.destination_address) {
    os << std::hex << (int)byte << " ";
  }
  os << std::endl;
  os << "  Source Address: ";
  for (auto byte : frame.source_address) {
    os << std::hex << (int)byte << " ";
  }
  os << std::endl;
  os << "  Digipeater Addresses: ";
  for (auto byte : frame.digipeater_addresses) {
    os << std::hex << (int)byte << " ";
  }
  os << std::endl;
  os << "  Control: " << std::hex << (int)frame.control << std::endl;
  os << "  Protocol ID: " << std::hex << (int)frame.protocol_id << std::endl;
  os << "  Information: ";
  for (auto byte : frame.information) {
    os << std::hex << (int)byte << " ";
  }
  os << std::endl;
  os << "  FCS: ";
  for (auto byte : frame.fcs) {
    os << std::hex << (int)byte << " ";
  }
  os << std::endl;
  os << "  Flag: " << std::hex << (int)frame.flag2 << std::endl;
  return os;
}


#endif  // AX25_H_