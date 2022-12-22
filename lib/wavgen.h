#ifndef WAVGEN_H_
#define WAVGEN_H_

#include <fstream>
#include <string>
#include <cmath>

class WavGen {
public:
    WavGen(std::string filename);
    ~WavGen();
    void addSineWave(int freq, float amp, float duration);
    void addSample(double sample);
    bool done();

private:
    void writeBytes(int data, int size); // Write the bytes to the file. 
                       //Automatically takes care of 16 bytes to chars conversion

    bool file_open_ = false;

    const int sample_rate_ = 44100; // The number of samples per second.
    const int bits_per_sample_ = 16; // The number of bits per sample.
    const int max_amplitude_ = pow(2, bits_per_sample_ - 1) - 1; // The maximum amplitude value

    float wave_angle_ = 0.0f; // Angle of the sine wave, needs to be persistent to get a continuous wave

    int data_start_ = 0; // The position of the start of the data chunk plus 10 bytes.
    int data_end_ = 0; // The position of the end of the data chunk.

    std::ofstream wav_file_ = std::ofstream();
};

#endif