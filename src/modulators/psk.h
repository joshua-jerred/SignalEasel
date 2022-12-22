#ifndef PSK_H_
#define PSK_H_

#include "modulators.h"
#include "bit-stream.h"

class PSK {
    public:
        enum Mode { 
            BPSK,
            QPSK
            };

        enum SymbolRate {
            S31, 
            S63, 
            S125, 
            S250, 
            S500,
            S1000
        };

        PSK(WavGen &wavgen, Mode mode, SymbolRate sym_rate);
        ~PSK();

        bool encodeTextData(std::string message);
        bool encodeRawData(unsigned char *data, int length);
        
    private:
        void setup(Mode mode, SymbolRate baud);
        
        const int preamble_length_ = 64; // symbols
        const int postamble_length_ = 64; // symbols

        Mode mode_;

        // Bit Stream members and methods
        void addVaricode(char c);
        
        
        void addPreamble();
        void addPostamble();

        // Modulation members and methods
        void encodeBitStream();
        void addSymbol(double shift, int next_shift);

        double symbol_rate_; // Symbol rate of the PSK modulation in Sym/s (125, 250, 500)
        int carrier_freq_ = 1500; // Carrier frequency in Hz (1500)
        int samples_per_symbol_; // floor(sample_rate_ / symbol_rate_)
        int sample_rate_ = 44100;

        double carrier_wave_angle_ = 0.0f;
        double angle_delta_;
        int last_symbol_end_filtered_ = 1;
        
        int last_bit_ = 0;
        unsigned char conv_code_buffer_ = 0;

        BitStream bit_stream_ = BitStream();
        WavGen &wavgen_;
};

#endif // PSK_H_
