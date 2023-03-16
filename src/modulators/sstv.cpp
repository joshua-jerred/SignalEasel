/**
 * @file sstv.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief SSTV Modulator, only compiled if SSTV_ENABLED is true
 * @date 2023-03-13
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#include <iostream>
#include <string>
#include <vector>

#include "modulators.h"
#include "mwav-exception.h"
#include "sstv-image-tools.h"

class RobotSstv {
 public:
  RobotSstv(WavGen &wavgen, SstvImage &image, const mwav::Sstv_Mode mode)
      : wavgen_(wavgen),
        image_(image),
        mode(mode),
        sample_rate_(wavgen.getSampleRate()) {
    switch (mode) {
      // case mwav::Sstv_Mode::ROBOT_12:
      //   color_format_ = ColorFormat::_4_2_0_;
      //   break;
      // case mwav::Sstv_Mode::ROBOT_24:
      //   color_format_ = ColorFormat::_4_2_0_;
      //   break;
      case mwav::Sstv_Mode::ROBOT_36:
        color_format_ = ColorFormat::_4_2_0_;
        line_width_ = 320;
        num_of_lines_ = 240;
        kYScanLineTime_ = 88.0;
        kCScanLineTime_ = 44.0;
        break;
      // case mwav::Sstv_Mode::ROBOT_72:
      //   color_format_ = ColorFormat::_4_2_2_;
      //   break;
      default:
        throw mwav::Exception("Mode not supported");
    }

    if (image_.GetWidth() != line_width_ ||
        image_.GetHeight() != num_of_lines_) {
      throw mwav::Exception("Image size does not match mode");
    }

    scan_line_buffer_.reserve(line_width_);
  }

  void Encode() {
    for (int i = 0; i < num_of_lines_; i++) {
      // for (int i = 0; i < 1; i++) {
      ReadImageLine(i);
      EncodeScanLine(i);
    }
  }

 private:
  enum class ColorFormat { _4_2_0_, _4_2_2_, NONE };
  enum class ColorType { Y, Cb, Cr };

  void ReadImageLine(int line_number) {
    if ((int)scan_line_buffer_.capacity() != line_width_) {
      throw mwav::Exception("Scan line buffer is not the correct size" +
                            std::to_string(scan_line_buffer_.capacity()) +
                            " != " + std::to_string(line_width_));
    }

    SstvImage::Pixel pixel;
    for (int i = 0; i < line_width_; i++) {
      if (!image_.GetPixel(i, line_number, pixel)) {
        throw mwav::Exception("Could not get pixel" + std::to_string(i) + "," +
                              std::to_string(line_number));
      }
      YCbCr ycbcr(pixel);

      scan_line_buffer_[i].y = ycbcr.y;
      if (color_format_ == ColorFormat::_4_2_2_) {
        scan_line_buffer_[i].cb = ycbcr.cb;
        scan_line_buffer_[i].cr = ycbcr.cr;
      } else if (color_format_ == ColorFormat::_4_2_0_) {
        if (line_number == 0) {  // First line, used to initialize the values.
          scan_line_buffer_[i].cb = ycbcr.cb;
          scan_line_buffer_[i].cr = ycbcr.cr;
        }
        if (line_number % 2 == 0) {  // Even Line
          scan_line_buffer_[i].cb = (scan_line_buffer_[i].cb + ycbcr.cb) / 2;
          scan_line_buffer_[i].cr = ycbcr.cr;
        } else {  // Odd Line
          scan_line_buffer_[i].cb = ycbcr.cb;
          scan_line_buffer_[i].cr = (scan_line_buffer_[i].cr + ycbcr.cr) / 2;
        }
      } else {
        throw mwav::Exception("Color format not supported");
      }
    }
  }

  void EncodeScanLine(int line_number) {
    bool even_line = line_number % 2 == 0;
    // bool even_line = true;

    // Start of Line
    Pulse(kSyncPulseLength_, kSyncPulseFrequency_);  // Sync Pulse

    Pulse(kSyncPorchLength_, kSyncPorchFrequency_);  // Sync Porch

    // Y Scan Line
    WriteBuffer(kYScanLineTime_, ColorType::Y);

    Pulse(kSeparationPulseLength_,
          even_line ? kEvenSeparationPulseFrequency_
                    : kOddSeparationPulseFrequency_);  // Separation Pulse
    Pulse(kPorchLength_, kPorchFrequency_);            // Porch

    if (even_line) {
      // R-Y Average Scan
      WriteBuffer(kCScanLineTime_, ColorType::Cr);
    } else {
      // B-Y Average Scan
      WriteBuffer(kCScanLineTime_, ColorType::Cb);
    }
  }

  /**
   * @brief
   * @param length in milliseconds
   * @param frequency in Hz
   */
  void Pulse(const double length, const int frequency) {
    int num_of_samples = std::ceil(((length / 1000.0) * (double)sample_rate_));
    wavgen_.addSineWaveSamples(frequency, mwav::constants::kAmplitude,
                               num_of_samples);
  }

  void WriteBuffer(const double total_time, RobotSstv::ColorType ycbcr_colors) {
    if (total_time <= 0) {
      throw mwav::Exception("Invalid total time");
    }
    int num_of_samples = (int)((total_time / 1000.0) * (double)sample_rate_);

    int previous_pixel_index = -1;
    int frequency = 0;
    double color = 0;
    for (int i = 0; i < num_of_samples; i++) {
      int new_pixel_index =
          (int)((double)i / (double)num_of_samples * (double)line_width_);
      if (new_pixel_index != previous_pixel_index) {
        previous_pixel_index = new_pixel_index;
        switch (ycbcr_colors) {
          case RobotSstv::ColorType::Y:
            color = scan_line_buffer_[previous_pixel_index].y;
            break;
          case RobotSstv::ColorType::Cb:
            color = scan_line_buffer_[previous_pixel_index].cb;
            break;
          case RobotSstv::ColorType::Cr:
            color = scan_line_buffer_[previous_pixel_index].cr;
            break;
          default:
            throw mwav::Exception("Invalid color type");
        }
        frequency = ColorToFreq(color);
      }
      wavgen_.addSineWaveSamples(frequency, mwav::constants::kAmplitude, 1);
    }
  }

  inline int ColorToFreq(const double color) {
    if (color < 0 || color > 255) {
      throw mwav::Exception("Invalid color value" + std::to_string(color));
    }
    return 1500.0 + (color * 3.1372549);
  }

  struct YCbCr {    // All values are 0-255
    double y = 0;   // Luminance
    double cb = 0;  // Chrominance Blue
    double cr = 0;  // Chrominance Red

    YCbCr() {}
    YCbCr(double r, double g, double b) { CalculateYCbCr(r, g, b); }
    YCbCr(const SstvImage::Pixel &pixel) {
      CalculateYCbCr(pixel.r, pixel.g, pixel.b);
    }
    void CalculateYCbCr(double r, double g, double b) {
      if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        throw mwav::Exception("Invalid RGB value");
      }
      y = 16.0 + (0.003906 * ((65.738 * r) + (129.057 * g) + (25.064 * b)));
      cb = 128.0 + (0.003906 * ((-37.945 * r) + (-74.494 * g) + (112.439 * b)));
      cr = 128.0 + (0.003906 * ((112.439 * r) + (-94.154 * g) + (-18.285 * b)));
      // y = r;
      // cr = 127;
      // cb = b;
    }
  };

  // Data Members ---------------------------------------------------
  /* - - - - - - - - - - */
  /* Robot Specification */
  /* - - - - - - - - - - */
  // All times are in milliseconds, all frequencies are in Hz
  ColorFormat color_format_ = ColorFormat::NONE;
  std::vector<YCbCr> scan_line_buffer_ = {};

  int line_width_ = 0;
  int num_of_lines_ = 0;

  const double kSyncPulseLength_ = 9;
  const double kSyncPulseFrequency_ = 1200;

  const double kSyncPorchLength_ = 3.0;
  const double kSyncPorchFrequency_ = 1500;

  double kYScanLineTime_ = 0;
  double kCScanLineTime_ = 0;

  const double kSeparationPulseLength_ = 4.5;
  const int kEvenSeparationPulseFrequency_ = 1500;
  const int kOddSeparationPulseFrequency_ = 2300;

  const double kPorchLength_ = 1.5;
  const double kPorchFrequency_ = 1900;

  /* - - - - - - - - - - */
  /* - - - - - - - - - - */

  WavGen &wavgen_;
  SstvImage &image_;
  const mwav::Sstv_Mode mode;

  // Waveform Generation
  const int sample_rate_;
};

bool modulators::SstvEncode(WavGen &wavgen, const std::string &input_image_path,
                            const std::string &callsign,
                            const mwav::Sstv_Mode mode,
                            const std::vector<std::string> &comments,
                            std::string output_image_path,
                            const bool save_out_image) {
  SstvImage::Mode sstv_image_mode;
  switch (mode) {
    case mwav::Sstv_Mode::ROBOT_36:
      sstv_image_mode = SstvImage::Mode::ROBOT_36_COLOR;
      break;
    default:
      throw mwav::Exception("Mode currently not supported");
  };

  if (save_out_image && output_image_path == "") {
    output_image_path = "sstv-" + input_image_path;
  }

  try {
    SstvImage image =
        SstvImage(sstv_image_mode, input_image_path, output_image_path);

    if (callsign != "NOCALLSIGN") {
      image.AddCallSign(callsign);
    }
    if (comments.size() > 0) {
      image.AddText(comments);
    }
    if (save_out_image) {
      image.Write();
    }
    image.AdjustColors();
    RobotSstv robot(wavgen, image, mode);
    robot.Encode();
  } catch (SstvImageToolsException &e) {
    throw mwav::Exception("Error modifying input image: " +
                          std::string(e.what()));
  }

  return true;
}