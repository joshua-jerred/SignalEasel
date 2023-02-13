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

std::ostream& operator<<(std::ostream& os, const AX25UiFrame& frame) {
  os << "AX25 Frame: " << std::endl;

  os << "0x" << std::hex << (int)frame.flag;
  std::cout << "|--|";
  for (int i = 0; i < 7; i++) {
    os << (char)(frame.destination_address[i] >> 1);
  }

  std::cout << "|--|";

  for (int i = 0; i < 7; i++) {
    os << (char)(frame.source_address[i] >> 1);
  }

  std::cout << "|--|";

  for (unsigned int i = 0; i < frame.digipeater_addresses.size(); i++) {
    os << (char)(frame.digipeater_addresses[i] >> 1);
  }

  std::cout << "|--|";

  os << "0x" << std::hex << (int)frame.control;

  std::cout << "|--|";

  os << "0x" << std::hex << (int)frame.protocol_id;
  std::cout << std::endl;

  for (unsigned int i = 0; i < frame.information.size(); i++) {
    os << (char)(frame.information[i] >> 1);
  }

  std::cout << std::endl;

  os << "0x" << std::hex << (int)frame.fcs[0] << " 0x" << std::hex
     << (int)frame.fcs[1];

  std::cout << "|--|";

  os << "0x" << std::hex << (int)frame.flag2 << std::endl;
  return os;
}


#endif  // AX25_H_