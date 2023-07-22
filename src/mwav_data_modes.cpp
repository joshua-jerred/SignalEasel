/**
 * @file data_modulation.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Data modulation functions.
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#include <wav_gen.hpp>

#include "data_modes.hpp"
#include "demodulators.hpp"
#include "modulators.hpp"

namespace mwav::data {

void encodeString(Mode modulation_mode, const std::string &input,
                  const std::string &out_file_path,
                  const std::string &call_sign) {
  wavgen::Generator wavgen = wavgen::Generator(out_file_path);

  if (modulation_mode == Mode::AFSK1200) {
    modulators::afskEncodeAscii(wavgen, input);
  } else {
    modulators::pskEncodeAscii(wavgen, input, modulation_mode);
  }
  // addCall(wavgen, call_sign);
  (void)call_sign;
}

bool decodeString(Mode modulation_mode, const std::string &in_file_path,
                  std::string &output) {
  wavgen::Reader wavgen = wavgen::Reader(in_file_path);
  if (modulation_mode == Mode::AFSK1200) {
    return demodulators::afskDecodeAscii(wavgen, output);
    return false;
  } else {
    return demodulators::pskDecodeAscii(wavgen, output, modulation_mode);
  }
}

} // namespace mwav::data