/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   sstv_modulator.cpp
 * @date   2024-06-08
 * @brief  SSTV implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/constants.hpp>
#include <SignalEasel/exception.hpp>
#include <SignalEasel/sstv.hpp>

namespace signal_easel::sstv {

Modulator::Modulator(sstv::Settings settings) : settings_(std::move(settings)) {
  switch (settings.mode) {
  case Settings::SstvMode::ROBOT36_COLOR:
    sstv_image_tools_mode = SstvImage::Mode::ROBOT_36_COLOR;
    color_format_ = ColorFormat::_4_2_0_;
    line_width_ = 320;
    num_of_lines_ = 240;
    y_scan_line_time_ = 88.0;
    c_scan_line_time_ = 44.0;

    sync_pulse_length_ms_ = 9;
    sync_pulse_frequency_ = 1200;

    sync_porch_length_ms_ = 3.0;
    sync_porch_frequency_ = 1500;

    separation_pulse_length_ms_ = 4.5;
    even_separation_pulse_frequency_ = 1500;
    odd_separation_pulse_frequency_ = 2300;

    porch_length_ms_ = 1.5;
    porch_frequency_ = 1900;
    break;

  default:
    throw Exception(Exception::Id::SSTV_MODE_NOT_IMPLEMENTED);
  }
}

void Modulator::encodeImage(std::string input_image_path) {
  try {
    SstvImage image = SstvImage(sstv_image_tools_mode, input_image_path);
    if (settings_.overlay_call_sign) {
      image.AddCallSign(settings_.call_sign);
    }

    image.AdjustColors();

    if (static_cast<uint32_t>(image.GetWidth()) != line_width_ ||
        static_cast<uint32_t>(image.GetHeight()) != num_of_lines_) {
      throw Exception(Exception::Id::SSTV_IMAGE_SIZE_MISMATCH);
    }

    scan_line_buffer_.reserve(line_width_);

    encodeAdjustedImage(image);
  } catch (const std::exception &e) {
    throw Exception(Exception::Id::SSTV_IMAGE_TOOLS_ERROR, e.what());
  }
}

void Modulator::encodeAdjustedImage(SstvImage &image) {
  for (uint32_t i = 0; i < num_of_lines_; i++) {
    // for (int i = 0; i < 1; i++) {
    readImageLine(image, i);
    encodeScanLine(i);
  }
}

void Modulator::readImageLine(SstvImage &image, uint32_t line_number) {
  if (scan_line_buffer_.capacity() != line_width_) {
    throw Exception(Exception::Id::SSTV_SCAN_LINE_BUFFER_SIZE_MISMATCH,
                    std::to_string(scan_line_buffer_.capacity()) +
                        " != " + std::to_string(line_width_));
  }

  SstvImage::Pixel pixel;
  for (uint32_t i = 0; i < line_width_; i++) {
    if (!image.GetPixel(i, line_number, pixel)) {
      throw Exception(Exception::Id::SSTV_GET_PIXEL_FAILED,
                      "px:" + std::to_string(i) + ", line" +
                          std::to_string(line_number));
    }
    YCbCr ycbcr(pixel);

    scan_line_buffer_[i].y = ycbcr.y;
    if (color_format_ == ColorFormat::_4_2_2_) {
      scan_line_buffer_[i].cb = ycbcr.cb;
      scan_line_buffer_[i].cr = ycbcr.cr;
    } else if (color_format_ == ColorFormat::_4_2_0_) {
      if (line_number == 0) { // First line, used to initialize the values.
        scan_line_buffer_[i].cb = ycbcr.cb;
        scan_line_buffer_[i].cr = ycbcr.cr;
      }
      if (line_number % 2 == 0) { // Even Line
        scan_line_buffer_[i].cb = (scan_line_buffer_[i].cb + ycbcr.cb) / 2;
        scan_line_buffer_[i].cr = ycbcr.cr;
      } else { // Odd Line
        scan_line_buffer_[i].cb = ycbcr.cb;
        scan_line_buffer_[i].cr = (scan_line_buffer_[i].cr + ycbcr.cr) / 2;
      }
    } else {
      throw Exception(Exception::Id::SSTV_COLOR_FORMAT_NOT_IMPLEMENTED);
    }
  }
}

void Modulator::encodeScanLine(uint32_t line_number) {
  bool even_line = line_number % 2 == 0;

  // Start of Line
  pulse(sync_pulse_length_ms_, sync_pulse_frequency_); // Sync Pulse

  pulse(sync_porch_length_ms_, sync_porch_frequency_); // Sync Porch

  // Y Scan Line
  writeBuffer(y_scan_line_time_, ColorType::Y);

  pulse(separation_pulse_length_ms_,
        even_line ? even_separation_pulse_frequency_
                  : odd_separation_pulse_frequency_); // Separation Pulse
  pulse(porch_length_ms_, porch_frequency_);          // Porch

  if (even_line) {
    // R-Y Average Scan
    writeBuffer(c_scan_line_time_, ColorType::Cr);
  } else {
    // B-Y Average Scan
    writeBuffer(c_scan_line_time_, ColorType::Cb);
  }
}

void Modulator::pulse(const double length, const int frequency) {
  int num_of_samples =
      std::ceil(((length / 1000.0) *
                 static_cast<double>(signal_easel::AUDIO_SAMPLE_RATE)));

  addSineWave(frequency, num_of_samples);
  // wavgen_.addSineWaveSamples(frequency, mwav::constants::kAmplitude,
  //  num_of_samples);
}

inline int ColorToFreq(const double color) {
  if (color < 0 || color > 255) {
    throw Exception(Exception::Id::SSTV_INVALID_RGB_VALUE,
                    std::to_string(color));
  }
  return 1500.0 + (color * 3.1372549);
}

void Modulator::writeBuffer(const double total_time, ColorType ycbcr_colors) {
  int num_of_samples =
      (int)((total_time / 1000.0) * (double)signal_easel::AUDIO_SAMPLE_RATE);

  int previous_pixel_index = -1;
  int frequency = 0;
  double color = 0;
  for (int i = 0; i < num_of_samples; i++) {
    int new_pixel_index =
        (int)((double)i / (double)num_of_samples * (double)line_width_);
    if (new_pixel_index != previous_pixel_index) {
      previous_pixel_index = new_pixel_index;
      switch (ycbcr_colors) {
      case ColorType::Y:
        color = scan_line_buffer_[previous_pixel_index].y;
        break;
      case ColorType::Cb:
        color = scan_line_buffer_[previous_pixel_index].cb;
        break;
      case ColorType::Cr:
        color = scan_line_buffer_[previous_pixel_index].cr;
        break;
      default:
        throw Exception(Exception::Id::SSTV_INVALID_COLOR_TYPE);
      }
      frequency = ColorToFreq(color);
    }

    addSineWave(frequency, 1);

    // wavgen_.addSineWaveSamples(frequency, mwav::constants::kAmplitude, 1);
  }
}

} // namespace signal_easel::sstv