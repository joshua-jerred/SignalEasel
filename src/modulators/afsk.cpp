//#include "afsk.h"
#include "modulators.h"

#include "bit-stream.h"
#include "wavgen.h"

#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>
#include <bitset>
#include <queue>

class AFSK {
 public:
    enum class MODE {
         AX25 = 0,
         MINIMODEM = 1
    };
    AFSK(WavGen &wavgen, AFSK::MODE mode) : wavgen_(wavgen), mode_(mode) {}
    ~AFSK() {}

    bool encodeRawData(unsigned char *data, int length); // Probably an AX.25 frame (with bit stuffing)
    bool encodeAscii(const std::string &message);

 private:
   WavGen &wavgen_;
   const MODE mode_; // Change this
   BitStream bit_stream_ = BitStream();
    // ------------------------------
    // AFSK Modulation Stuff
    // ------------------------------
    void encodeBitStream();
    void addSymbol(bool change);

    // Constants
    const int baud_rate_ = 1200; // 1200 Baud rate of the AFSK signal
    const int freq_center_ = 1700; // 1700 Center frequency of the AFSK signal
    const int freq_dev_ = 500; // 500 Frequency deviation for mark/space

    // Variables
    bool last_bit_ = false; // Used for modulation (NRZI)
};

bool AFSK::encodeRawData(unsigned char *data, int length) { // length is in bytes
    for (int i = 0; i < length; i++) {
        bit_stream_.addBits(data + i, 8);
    }
    bit_stream_.pushBufferToBitStream();

    encodeBitStream();
    return true;
}

void AFSK::encodeBitStream() {
    // NRZI encoding (Non Return to Zero Inverted)
    // Most examples online get this wrong even for AFSK1200
    // 0 is encoded as a *change in tone*, while 1 is encoded as *no change*
    // Bit stuffing comes before this
    
    // constants
    // baud_rate_ = 1200
    // freq_center_ = 1700
    // freq_dev_ = 500
    // sample_rate_ = 44100
    // sample_freq_ = 44100 * 2
    // samples_per_bit_ = sample_freq_ / baud_rate_ = 147 

    const int sample_freq_ = wavgen_.getSampleRate() * 4; // 176400
    const int samples_per_bit_ = sample_freq_/baud_rate_; // 147
    const int bits_to_encode_ = bit_stream_.getBitStreamLength();

    const double f_center_over_time = (double) freq_center_/ (double)sample_freq_;
    const double f_delta_over_time = (double) freq_dev_/(double)sample_freq_;

    //std::cout << "Encoding bit stream" << std::endl;
    //std::cout << "Baud: " << baud_rate_ << " fC: " << freq_center_ 
    //    << " fD: " << freq_dev_ << " sR: " << sample_rate_ 
    //    << " sF: " << sample_freq_ << " sPB: " << samples_per_bit_
    //    << " Bits to encode: " << bit_stream_length_ << std::endl;
    //std::cout << "fC/t: " << f_center_over_time << " fD/t: " << f_delta_over_time << std::endl;
    
    /**
     * @brief NRZI encoding
     * @details 0 is encoded as a change in tone, while 1 is encoded as no change
     * It's not encoded into 0 and 1, it's bipolar, so -1 and 1.
     */
    std::queue<int8_t> nrzi_bipolar_bits_;
    
    int current_bit = bit_stream_.popNextBit();
    if (mode_ == MODE::AX25) {
        int current = 1; // 1 = mark, -1 = space
        while (bit_stream_.getBitStreamLength() >= 0) {
            if (!current_bit) { // 0
                current = -current;
            }
            nrzi_bipolar_bits_.push(current);
            current_bit = bit_stream_.popNextBit();
            if (current_bit == -1) {
                break;
            }
        }
    } else if (mode_ == MODE::MINIMODEM) {
        while (bit_stream_.getBitStreamLength() >= 0) {
            if (!current_bit) { // 0
                nrzi_bipolar_bits_.push(1);
            } else { // 1
                nrzi_bipolar_bits_.push(-1);
            }
            
            current_bit = bit_stream_.popNextBit();
            if (current_bit == -1) {
                break;
            }
        }
    }

    int m = 0;
    int index = 0;
    int prev_index = 0;

    int current_bp_bit = nrzi_bipolar_bits_.front(); // Current bipolar bit
    int prev_bp_bit = current_bp_bit; // Previous bipolar bit
    nrzi_bipolar_bits_.pop();

    int iterations = (bits_to_encode_)*samples_per_bit_;
    for (int i = 2; i < iterations; i++) {
        index = floor((float)i / (float)samples_per_bit_);
        prev_index = floor((float)(i - 1) / (float)samples_per_bit_);

        if (index != prev_index) {
            prev_bp_bit = current_bp_bit;
            current_bp_bit = nrzi_bipolar_bits_.front();
            nrzi_bipolar_bits_.pop();
        } else {
            prev_bp_bit = current_bp_bit;
        }
        m += ((current_bp_bit + prev_bp_bit)/2);
        double lhs = 2.0 * M_PI * (double)i * f_center_over_time;
        double rhs = 2.0 * M_PI * (double)m * f_delta_over_time;
        double wave = cos(lhs + rhs);
        double sample = wave;
        if (i % 4 == 0) { // Only write every 4th sample, since we're using 4x oversampling for AFSK allignment
            wavgen_.addSample(sample);
        }
    }
}

bool AFSK::encodeAscii(const std::string &message) {
    return encodeRawData((unsigned char *)message.c_str(), message.length());
}

bool modulators::AfskAscii(WavGen &wavgen, const std::string &message) {
    AFSK afsk(wavgen, AFSK::MODE::MINIMODEM);
    return afsk.encodeAscii(message);
}

bool modulators::AfskBinary(WavGen &wavgen, const std::vector<uint8_t> &data) {
    AFSK afsk(wavgen, AFSK::MODE::AX25);
    return afsk.encodeRawData((unsigned char *)data.data(), data.size());
}