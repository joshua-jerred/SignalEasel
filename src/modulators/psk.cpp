
#include <string>
#include <fstream>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <math.h>
#include <exception>
#include <sstream>
#include <iostream> // Debugging

#include "convolutional-code.h"
#include "varicode.h"
#include "psk.h"

PSK::PSK(WavGen &wavgen, Mode mode, SymbolRate sym_rate):
    wavgen_(wavgen) {
    setup(mode, sym_rate);
}

PSK::~PSK() {

}

bool PSK::encodeTextData(std::string message) {

    addPreamble(); // Add PSK Preamble to bitstream (0's)
    for (char &c : message) {
        addVaricode(c); // Add each char of the message to the bitstream (varicode)
        static unsigned char zeros[1] = {0x00};
        bit_stream_.addBits(zeros, 2); // Add two 0's between each character
    }
    addPostamble(); // Add PSK Postamble to bitstream
    bit_stream_.pushBufferToBitStream(); // Push any remaining bits in the buffer to the bitstream

    encodeBitStream(); // Encode the bitstream into PSK audio, write to wav file

    //if (morse_callsign_) { // If callsign specified, add morse code callsign to end of audio
    //    addCallSign();
    //}
    return true;
}

bool PSK::encodeRawData(unsigned char *data, int length) {
    addPreamble(); // Add PSK Preamble to bitstream
    //addBits(data, length*8); // Add raw data to bitstream
    addPostamble(); // Add PSK Postamble to bitstream
    encodeBitStream(); // Encode the bitstream into PSK audio, write to wav file
    return true;
}

void PSK::setup(Mode mode, SymbolRate symbol_rate) {
    mode_ = mode;
    switch (mode) {
        case BPSK:
            break;
        case QPSK:
            break;
        default:
            throw std::invalid_argument("Invalid mode");
            break;
    }

    switch (symbol_rate) {
        case S31:
            symbol_rate_ = 31.5;
            break;
        case S63:
            symbol_rate_ = 63.5;
            break;
        case S125:
            symbol_rate_ = 125.0;
            break;
        case S250:
            symbol_rate_ = 250.0;
            break;
        case S500:
            symbol_rate_ = 500.0;
            break;
        case S1000:
            symbol_rate_ = 1000.0;
            break;
        default:
            throw std::invalid_argument("Invalid symbol rate");
            break;
    }

    angle_delta_ = 2.0 * M_PI * ( (double) carrier_freq_ / (double) sample_rate_ );
    samples_per_symbol_ = std::floor(sample_rate_ / symbol_rate_);
}

void PSK::addVaricode(char c) {
    uint16_t varicode = ascii_to_varicode[c];
    //std::cout << std::endl << "Adding varicode for " << c << ":" << std::bitset<16>(varicode) << " ";
    unsigned char bits[2];
    int previous_bit = 1;
    for (int i = 0; i < 16; i++) {
        if (!(varicode & (1 << i)) && (!previous_bit)) {
            //std::cout << "size: " << i - 1;

            bits[0] = i > 9 ? varicode >> 1 : varicode << (9 - i);
            bits[1] = i > 9 ? varicode << (17 - i) : 0x00;

            //std::cout << " shifted: " << std::bitset<8>(bits[0]) << " " << std::bitset<8>(bits[1]) << std::endl;
            bit_stream_.addBits(bits, i - 1);
            break;
        } else {
            previous_bit = varicode & (1 << i);
        }
    }
}

void PSK::addPreamble() {
    static unsigned char zeros[1] = {0x00};
    for (int i = 0; i < preamble_length_; i++) {
        bit_stream_.addBits(zeros, 1);
    }
}

void PSK::addPostamble() {
    const int fldigi_postamble_mode_ = 0;
    static unsigned char zeros[1] = {0x00};
    static unsigned char ones[1] = {0xFF};
    for (int i = 0; i < postamble_length_; i++) {
        bit_stream_.addBits((mode_ == QPSK && fldigi_postamble_mode_ ? zeros : ones), 1);
    }
}

// Modulation Methods
/**
 * @brief Goes throughout he bit stream and modulates with the addSymbol method.
 * @details With BPSK31, the bit 0 is encoded by switching phases and the bit 1 
 * is encoded by keeping the phase shift the same.
 */
void PSK::encodeBitStream() {
    if (mode_ == BPSK) { // BPSK modulation
        int bit = bit_stream_.popNextBit();
        int next_bit = bit_stream_.peakNextBit();
        int last_phase = 0; // 0 = 0, 1 = M_PI
        while (bit != -1) { 
            if (bit) { // Encode a 1 by keeping the phase shift the same
                int filter_end = next_bit == 1 ? 0 : 1; // If next bit is 1, do not filter end of symbol.
                addSymbol(last_phase ? 0 : M_PI, filter_end);
            } else if (!bit) { // Encode a 0 by switching phase
                int filter_end = next_bit == 1 ? 0 : 1; // If next bit is 1, do not filter end of symbol.
                addSymbol(last_phase ? M_PI : 0, filter_end);
                last_phase = !last_phase;
            }
            last_phase ? "1" : "0";
            bit = bit_stream_.popNextBit();
            next_bit = bit_stream_.peakNextBit();
        }
    } else if (mode_ == QPSK) { // QPSK modulation
        unsigned char buffer = 0;
        int bit = bit_stream_.popNextBit();
        int shift = 0;
        while (bit != -1) {
            buffer = ((buffer << 1) | bit) & 0x1f;
            int filter_end = 0;
            if (shift != conv_code[buffer]) {
                filter_end = 1;
            }
            shift += conv_code[buffer];
            addSymbol(shift, filter_end);
            bit = bit_stream_.popNextBit();
        }
    }
}

/**
 * @brief Modulates a single symbol in BPSK/QPSK and saves the audio data to the
 * wav file.
 * 
 * @param shift The shift of the carrier wave in degrees
 * @param filter_end Whether or not to apply the filter to the end of the symbol
 */
void PSK::addSymbol(double shift, int filter_end) {
    const double power = 2.0;
    const double roll_off = 2.9;
    const double amplitude = .5;

    double time = 0 - (samples_per_symbol_ / 2);
    for (int i = 0; i < samples_per_symbol_; i++) {
        double unfiltered = std::cos(carrier_wave_angle_ + shift);
        double filter = std::pow(std::cos( (abs(time) / samples_per_symbol_) * roll_off ), power);
        if (!last_symbol_end_filtered_ && (time < 0)) {
            filter = 1;
        }
        if (!filter_end && (time > 0)) { // Remove filter from end of symbol
            filter = 1;
        }
        double sample = amplitude * filter * unfiltered;
        wavgen_.addSample(sample); // write sample to wav file

        carrier_wave_angle_ += angle_delta_;
        time += 1;
        if (carrier_wave_angle_ > 2 * M_PI) {
            carrier_wave_angle_ -= 2 * M_PI;
        }
    }
    last_symbol_end_filtered_ = filter_end;
}

bool addPskASCII(WavGen &wavgen, std::string message) {
    PSK psk(wavgen, PSK::Mode::BPSK, PSK::SymbolRate::S125);
    psk.encodeTextData(message);
    return true;
}