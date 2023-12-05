/**
 * @file psk.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief AFSK Demodulator
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include "afsk.hpp"
#include "bit-stream.h"
#include "demodulators.hpp"
#include "mwav.hpp"

bool afskBitStreamToAscii(BitStream &bit_stream, std::string &output) {}

bool demodulators::afskDecodeAscii(wavgen::Reader &wavgen_reader,
                                   std::string &message) {
  std::vector<int8_t> base_band_signal;
  BitStream bit_stream;
  bool success = false;

  afskSignalToBaseBand(wavgen_reader, base_band_signal);
  afskBaseBandToBitStream(base_band_signal, bit_stream);
  success = afskBitStreamToAscii(bit_stream, message);
  std::cout << std::endl << message << std::endl;

  return success;
}