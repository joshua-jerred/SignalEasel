#include <cmath>

#include "bit-stream.h"
#include "convolutional-code.h"
#include "modulators.h"
#include "mwav-exception.h"
#include "varicode.h"

struct PskWave {
  double carrier_wave_angle = 0.0f;
  int last_symbol_end_filtered = 1;
  double angle_delta = 0.0f;
  int samples_per_symbol = 0;
};

enum class PskMode { BPSK, QPSK };

PskMode GetPskMode(const mwav::DataModulation modulate_type) {
  switch (modulate_type) {
    case mwav::DataModulation::BPSK_125:
    case mwav::DataModulation::BPSK_250:
    case mwav::DataModulation::BPSK_500:
    case mwav::DataModulation::BPSK_1000:
      return PskMode::BPSK;
    case mwav::DataModulation::QPSK_125:
    case mwav::DataModulation::QPSK_250:
    case mwav::DataModulation::QPSK_500:
      return PskMode::QPSK;
    default:
      throw mwav::Exception("Invalid modulation type for PSK");
  }
}

double GetSymbolRate(const mwav::DataModulation modulate_type) {
  switch (modulate_type) {
    case mwav::DataModulation::BPSK_125:
    case mwav::DataModulation::QPSK_125:
      return 125.0;
    case mwav::DataModulation::BPSK_250:
    case mwav::DataModulation::QPSK_250:
      return 250.0;
    case mwav::DataModulation::BPSK_500:
    case mwav::DataModulation::QPSK_500:
      return 500.0;
    case mwav::DataModulation::BPSK_1000:
      return 1000.0;
    default:
      throw mwav::Exception("Invalid modulation type for PSK");
  }
}

void addVaricode(BitStream &bit_stream, char c) {
  uint16_t varicode = ascii_to_varicode[(int)c];
  unsigned char bits[2];
  int previous_bit = 1;
  for (int i = 0; i < 16; i++) {
    if (!(varicode & (1 << i)) && (!previous_bit)) {
      bits[0] = i > 9 ? varicode >> (i - 9)
                      : varicode << (9 - i);  // i > 9 = more than 8 bits
      bits[1] = i > 9 ? varicode << (17 - i) : 0x00;
      bit_stream.addBits(bits, i - 1);
      break;
    } else {
      previous_bit = varicode & (1 << i);
    }
  }
}

void addPreamble(BitStream &bit_stream, int preamble_length) {
  static unsigned char zeros[1] = {0x00};
  for (int i = 0; i < preamble_length; i++) {
    bit_stream.addBits(zeros, 1);
  }
}

void addPostamble(BitStream &bit_stream, int postamble_length, PskMode mode) {
  const bool kFldigiMode = true; // Fldigi uses 0x00 for QPSK
  static unsigned char zeros[1] = {0x00};
  static unsigned char ones[1] = {0xFF};
  postamble_length +=
      32 - (bit_stream.getBitStreamLength() % 32);  // pad to 32 bits
  for (int i = 0; i < postamble_length; i++) {
    bit_stream.addBits(
        ((mode == PskMode::QPSK && kFldigiMode) ? zeros : ones), 1);
  }
}

void addSymbol(WavGen &wavgen, PskWave &wave, double shift, int filter_end) {
  const double power = 2.0;
  const double roll_off = 2.9;
  const double amplitude = .5;

  double time = 0 - (wave.samples_per_symbol / 2);
  for (int i = 0; i < wave.samples_per_symbol; i++) {
    double unfiltered = std::cos(wave.carrier_wave_angle + shift);
    double filter = std::pow(
        std::cos((std::abs(time) / wave.samples_per_symbol) * roll_off), power);
    if (!wave.last_symbol_end_filtered && (time < 0)) {
      filter = 1;
    }
    if (!filter_end && (time > 0)) {  // Remove filter from end of symbol
      filter = 1;
    }
    double sample = amplitude * filter * unfiltered;
    wavgen.addSample(sample);  // write sample to wav file

    wave.carrier_wave_angle += wave.angle_delta;
    time += 1;
    if (wave.carrier_wave_angle > 2 * M_PI) {
      wave.carrier_wave_angle -= 2 * M_PI;
    }
  }
  wave.last_symbol_end_filtered = filter_end;
}

void encodeBpsk(WavGen &wavgen, BitStream &bit_stream, PskWave &wave) {
  int bit = bit_stream.popNextBit();
  int next_bit = bit_stream.peakNextBit();
  int last_phase = 0;  // 0 = 0, 1 = M_PI
  while (bit != -1) {
    if (bit) {  // Encode a 1 by keeping the phase shift the same
      int filter_end =
          next_bit == 1 ? 0
                        : 1;  // If next bit is 1, do not filter end of symbol.
      addSymbol(wavgen, wave, last_phase ? 0 : M_PI, filter_end);
    } else if (!bit) {  // Encode a 0 by switching phase
      int filter_end =
          next_bit == 1 ? 0
                        : 1;  // If next bit is 1, do not filter end of symbol.
      addSymbol(wavgen, wave, last_phase ? M_PI : 0, filter_end);
      last_phase = !last_phase;
    }
    // last_phase ? "1" : "0";
    bit = bit_stream.popNextBit();
    next_bit = bit_stream.peakNextBit();
  }
}

void encodeQpsk(WavGen &wavgen, BitStream &bit_stream, PskWave &wave) {
  unsigned char buffer = 0;
  int bit = bit_stream.popNextBit();
  int shift = 0;
  while (bit != -1) {
    buffer = ((buffer << 1) | bit) & 0x1f;
    int filter_end = 0;
    if ((int)shift != (int)conv_code[buffer]) {
      filter_end = 1;
    }
    shift += conv_code[buffer];
    addSymbol(wavgen, wave, shift, filter_end);
    bit = bit_stream.popNextBit();
  }
}

bool modulators::PskEncodeAscii(WavGen &wavgen, const std::string &message,
                                const mwav::DataModulation &modulation_type) {
  BitStream bit_stream = BitStream();

  const int kCarrierFreq = 1500;    // PSK Carrier Frequency in Hz
  const int kSampleRate = 44100;    // Sample Rate in Hz
  const int kPreambleLength = 64;   // PSK Preamble Length in Symbols
  const int kPostambleLength = 64;  // PSK Postamble Length in Symbols

  float symbol_rate = GetSymbolRate(modulation_type);
  PskMode mode = GetPskMode(modulation_type);

  PskWave psk_wave = PskWave();
  psk_wave.carrier_wave_angle = 0.0f;
  psk_wave.last_symbol_end_filtered = 1;
  psk_wave.angle_delta =
      2.0 * M_PI * ((double)kCarrierFreq / (double)kSampleRate);
  psk_wave.samples_per_symbol = std::floor(kSampleRate / symbol_rate);

  /*Encode the message info binary*/
  addPreamble(bit_stream, kPreambleLength);
  for (const char &c : message) {  // Encode each character in the message to
                                   // the BitStream (varicode)
    addVaricode(bit_stream, c);
    static unsigned char zeros[1] = {0x00};
    bit_stream.addBits(zeros, 2);
  }
  addPostamble(bit_stream, kPostambleLength,
               mode);  // Mode is needed for 'Fdigi compatibility'
  bit_stream.pushBufferToBitStream();  // Push any remaining bits in the buffer
                                       // to the BitStream

  if (mode == PskMode::BPSK) {
    encodeBpsk(wavgen, bit_stream, psk_wave);
  } else if (mode == PskMode::QPSK) {
    encodeQpsk(wavgen, bit_stream, psk_wave);
  } else {
    return false;
  }

  return true;
}