/**
 * @file psk.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief PSK Demodulator
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#include "demodulators.hpp"

bool demodulators::pskDecodeAscii(wavgen::Reader &wavgen_reader,
                                  std::string &message,
                                  mwav::data::Mode modulation_type) {
  (void)wavgen_reader;
  (void)message;
  (void)modulation_type;
  return false;
}
