/**
 * @file sstv_modulation.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Public interface for aprs modulation.
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#ifndef MWAV_SSTV_MODULATION_HPP_
#define MWAV_SSTV_MODULATION_HPP_

namespace mwav::sstv {

#if SSTV_ENABLED

enum class Mode {
  ROBOT_36,
  ROBOT_72  // Currently not supported
};

bool EncodeSSTV(
    const std::string &out_file_path, const std::string &input_image_path,
    const bool save_out_image = false,
    const std::string &callsign = "NOCALLSIGN",
    const Sstv_Mode mode = Sstv_Mode::ROBOT_36,
    const std::vector<std::string> &comments = std::vector<std::string>(),
    std::string out_image_path = "", const bool morse_callsign = false);

#endif  // SSTV_ENABLED

}  // namespace mwav::sstv

#endif /* MWAV_SSTV_MODULATION_HPP_ */