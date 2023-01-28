/**
 * @file audio-in.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief A pulse audio input utility
 * @date 2023-01-27
 * @copyright Copyright (c) 2023
 * @version 0.1
 * 
 * @todo PulseAudio and FFTW3
 */

#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cmath>

#include <pulse/simple.h>
#include <pulse/error.h>


int main() {
    int error;
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = 44100;
    ss.channels = 1;

    pa_simple *s = pa_simple_new(NULL, "MWAV", PA_STREAM_RECORD, NULL, "Audio Decoding", &ss, NULL, NULL, &error);
    if (!s) {
        std::cerr << "Failed to connect to pulseaudio: " << pa_strerror(error) << std::endl;
        return 1;
    }

    std::vector<int16_t> buffer(1024);
    while (true) {
        if (pa_simple_read(s, buffer.data(), buffer.size() * sizeof(int16_t), &error) < 0) {
            std::cerr << "Failed to read from pulseaudio: " << pa_strerror(error) << std::endl;
            return 1;
        }

        for (int16_t sample : buffer) {
            std::cout << ((float)std::abs(sample))/32768.0f << std::endl;
        }
    }

    return 0;
}