/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   aprs_receiver.cpp
 * @date   2023-12-10
 * @brief  Implementation of the APRS receiver class.
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/aprs.hpp>
#include <SignalEasel/exception.hpp>

#include <iomanip>
#include <iostream>

namespace signal_easel {

void aprs::Receiver::decode() {
  auto demodulation_res = demodulator_.processAudioBuffer();

  std::cout << "Decoding [";
  std::cout << std::setprecision(2) << std::fixed
            << "SNR: " << demodulation_res.snr << "]" << std::endl;

  aprs_demodulator_.output_bit_stream_ = demodulator_.output_bit_stream_;
  bool res = false;
  try {
    res = aprs_demodulator_.lookForAx25Packet();
  } catch (Exception &e) {
    std::cout << "Exception: " << e.what() << std::endl;
    res = false;
  }

  if (res) {
    std::cout << aprs_demodulator_.frame_ << std::endl;
  }
}

} // namespace signal_easel