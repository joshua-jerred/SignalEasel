/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   sstv.hpp
 * @date   2024-06-08
 * @brief  SSTV implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_SSTV_HPP_
#define SIGNAL_EASEL_SSTV_HPP_

#include <string>

#include <SignalEasel/exception.hpp>
#include <SignalEasel/modulator.hpp>

#include "sstv-image-tools.h"

namespace signal_easel::sstv {

/**
 * @brief Settings for SSTV modulation
 */
struct Settings : public signal_easel::Settings {
  /**
   * @brief An enum for the different SSTV modes
   */
  enum class SstvMode { ROBOT36_COLOR };

  /**
   * @brief The mode of SSTV to use
   */
  SstvMode mode = SstvMode::ROBOT36_COLOR;

  /**
   * @brief True to overlay the call sign on the image, false to not
   */
  bool overlay_call_sign = true;
};

/**
 * @brief SSTV Modulator
 */
class Modulator : public signal_easel::Modulator {
public:
  /**
   * @brief Constructor for the SSTV Modulator
   * @param settings The settings for the modulator
   */
  Modulator(sstv::Settings settings = sstv::Settings());
  ~Modulator() = default;

  /**
   * @brief Encodes an image into SSTV audio
   * @param input_image_path The path to the image to encode
   * @exception signal_easel::Exception Thrown on failure
   */
  void encodeImage(std::string input_image_path);

private:
  enum class ColorFormat { _4_2_0_, _4_2_2_, NONE };
  enum class ColorType { Y, Cb, Cr };

  void encodeAdjustedImage(SstvImage &image);
  void readImageLine(SstvImage &image, uint32_t line_number);
  void encodeScanLine(uint32_t line_number);
  void pulse(double length, int frequency);
  void writeBuffer(const double total_time, ColorType ycbcr_colors);

  /**
   * @brief The settings for the modulator
   */
  sstv::Settings settings_;

  SstvImage::Mode sstv_image_tools_mode = SstvImage::Mode::ROBOT_36_COLOR;
  ColorFormat color_format_ = ColorFormat::NONE;
  uint32_t line_width_ = 0;
  uint32_t num_of_lines_ = 0;

  double y_scan_line_time_ = 0;
  double c_scan_line_time_ = 0;

  double sync_pulse_length_ms_ = 0;
  double sync_pulse_frequency_ = 0;
  double sync_porch_length_ms_ = 0;
  double sync_porch_frequency_ = 0;

  double separation_pulse_length_ms_ = 0;
  int even_separation_pulse_frequency_ = 0;
  int odd_separation_pulse_frequency_ = 0;

  double porch_length_ms_ = 0;
  double porch_frequency_ = 0;

  struct YCbCr {   // All values are 0-255
    double y = 0;  // Luminance
    double cb = 0; // Chrominance Blue
    double cr = 0; // Chrominance Red

    YCbCr() {}
    YCbCr(double r, double g, double b) { CalculateYCbCr(r, g, b); }
    YCbCr(const SstvImage::Pixel &pixel) {
      CalculateYCbCr(pixel.r, pixel.g, pixel.b);
    }
    void CalculateYCbCr(double r, double g, double b) {
      if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        throw Exception(Exception::Id::SSTV_INVALID_RGB_VALUE);
      }
      y = 16.0 + (0.003906 * ((65.738 * r) + (129.057 * g) + (25.064 * b)));
      cb = 128.0 + (0.003906 * ((-37.945 * r) + (-74.494 * g) + (112.439 * b)));
      cr = 128.0 + (0.003906 * ((112.439 * r) + (-94.154 * g) + (-18.285 * b)));
      // y = r;
      // cr = 127;
      // cb = b;
    }
  };

  std::vector<YCbCr> scan_line_buffer_ = {};
};

} // namespace signal_easel::sstv

#endif /* SIGNAL_EASEL_SSTV_HPP_ */