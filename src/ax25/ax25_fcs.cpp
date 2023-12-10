/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   ax25_fcs.cpp
 * @date   2023-12-09
 * @brief  AX.25 Frame Check Sequence (FCS) tools
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/ax25.hpp>

namespace signal_easel::ax25 {
uint16_t calculateFcs(const std::vector<uint8_t> &input_data) {
  uint16_t crc = 0xFFFF;
  uint16_t crc16_table[] = {0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285,
                            0x6306, 0x7387, 0x8408, 0x9489, 0xa50a, 0xb58b,
                            0xc60c, 0xd68d, 0xe70e, 0xf78f};

  size_t length = input_data.size();
  size_t iterator = 0;
  while (length--) {
    crc =
        (crc >> 4) ^ crc16_table[(crc & 0xf) ^ (input_data.at(iterator) & 0xf)];
    crc =
        (crc >> 4) ^ crc16_table[(crc & 0xf) ^ (input_data.at(iterator) >> 4)];
    iterator++;
  }

  // uint32_t data;
  // data = crc;
  // crc = (crc << 8) |
  // (data >> 8 & 0xff); // do byte swap here that is needed by AX25 standard
  return (~crc);
}
} // namespace signal_easel::ax25