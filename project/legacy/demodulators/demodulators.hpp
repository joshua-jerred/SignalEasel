/**
 * @file demodulators.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Demodulators for various modes
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#ifndef DEMODULATORS_HPP_
#define DEMODULATORS_HPP_

#include <wav_gen.hpp>

#include "data_modes.hpp"

namespace demodulators {

bool pskDecodeAscii(wavgen::Reader &wavgen_reader, std::string &message,
                    mwav::data::Mode modulation_type);
bool afskDecodeAscii(wavgen::Reader &wavgen_reader, std::string &message);

} // namespace demodulators

#endif /* DEMODULATORS_HPP_ */