/**
 * @file wavgen.cpp
 * @author Joshua Jerred
 * @brief See https://github.com/joshua-jerred/WavGen
 * @version 1.0
 * @date 2022-10-02
 * @copyright Copyright (c) 2022
 */

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>

#include "wavgen.h"

#define SINE_FILTER_SAMPLES 150 // Look at the sine wave in Audacity, you'll see the smoothing on sine waves

WavGen::WavGen(std::string filename) {
    wav_file_.open(filename, std::ios::binary);

    if (!wav_file_.is_open()) {
        throw std::runtime_error("Could not open file");
        file_open_ = false;
        return;
    } else {
        file_open_ = true;
    }

    wav_file_ << "RIFF****WAVE"; // RIFF header
    wav_file_ << "fmt "; // format
    writeBytes(16, 4); // size
    writeBytes(1, 2); // compression code
    writeBytes(1, 2); // number of channels
    writeBytes(sample_rate_, 4); // sample rate
    writeBytes(sample_rate_ * bits_per_sample_ / 8, 4 ); // Byte rate
    writeBytes(bits_per_sample_ / 8, 2); // block align
    writeBytes(bits_per_sample_, 2); // bits per sample
    wav_file_ << "data****"; // actual follows this

    data_start_ = wav_file_.tellp(); // Save the position of the start of the
                                     // data chunk
}

WavGen::~WavGen() {
    done();
}

void WavGen::addSineWave(int freq, float amp, float duration) {
    float offset = 2 * M_PI * freq / sample_rate_; // The offset of the angle
                                                   // between samples
    
    // These 2 values are persistent through the for Loop
    float amplitude = amp;
    
    int total_samples = std::floor(sample_rate_ * duration);
    float filter = 0.0f; // Filter to reduce the amplitude of the wav in the first and last SINE_FILTER_SAMPLES samples
    float filter_step = 1.0f / SINE_FILTER_SAMPLES; // The amount to increase the filter by each sample to get a smooth transition
    
    for(int i = 0; i < total_samples; i++ ) { // For each sample
        wave_angle_ += offset;
        int sample = static_cast<int> ((filter * amplitude * sin(wave_angle_)) * max_amplitude_);
        writeBytes(sample, 2);

        if (wave_angle_ > 2 * M_PI) {
            wave_angle_ -= 2 * M_PI;
        }

        if (i < SINE_FILTER_SAMPLES) { // Adjust the filter
            filter += filter_step;
        } else if (i > total_samples - SINE_FILTER_SAMPLES) {
            filter -= filter_step;
        } else {
            filter = 1.0f;
        }
    }
}

void WavGen::addSample(double sample) {
    if (sample > 1.0) {
        sample = 1.0;
    } else if (sample < -1.0) {
        sample = -1.0;
    }
    int sample_int = static_cast<int> (sample * max_amplitude_);
    writeBytes(sample_int, 2);
}

bool WavGen::done() {
    if (!file_open_) {
        return false;
    }
    data_end_ = wav_file_.tellp(); // Save the position of the end of the data
                                   // chunk

    wav_file_.seekp(data_start_ - 4); // Go to the beginning of the data chunk
    writeBytes(data_end_ - data_start_, 4); // and write the size of the chunk.
    wav_file_.seekp(4, std::ios::beg); // Go to the beginning of the file
    writeBytes(data_end_ - 8, 4); // Write the size of the overall file
    wav_file_.close();
    file_open_ = false;
    return true;
}

void WavGen::writeBytes(int data, int size) {
    if (file_open_) {
        wav_file_.write(reinterpret_cast<const char*> (&data), size);
    }
}