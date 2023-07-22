/**
 * @file data_modulation.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Data modulation functions.
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#include <wav_gen.hpp>

#include "data_modulation.hpp"
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

} // namespace mwav::data