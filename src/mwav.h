#ifndef MWAV_H_
#define MWAV_H_

#include "modulators.h"
#include "wavgen.h"
#include "aprs.h"

namespace MWAVData {

enum class MODE { ASCII = 0, RAW_BIN = 1 };

enum class MODULATION {
  BPSK_125,
  BPSK_250,
  BPSK_500,
  BPSK_1000,
  QPSK_125,
  QPSK_250,
  QPSK_500,
  AFSK1200
};

/**
 * @brief Encode a string of data into a WAV file, without a morse code
 * callsign. ASCII Input only.
 * @param modulation The modulation to use for the data, see
 * MWAVData::MODULATION
 * @param input The string to encode
 * @param out_file_path The path to the output WAV file to create
 * @return true if the WAV file was created successfully
 * @return false if the WAV file could not be created
 */
bool encode(const MODULATION modulation, const std::string input,
            const std::string out_file_path);

/**
 * @brief Encode a string of data into a WAV file, with a morse code callsign.
 * ASCII Input only.
 * @param modulation The modulation to use for the data, see
 * MWAVData::MODULATION
 * @param input The string to encode
 * @param out_file_path The path to the output WAV file to create
 * @param callsign The callsign to encode in morse code
 * @return true if the WAV file was created successfully
 * @return false if the WAV file could not be created
 */
bool encode(const MWAVData::MODULATION modulation, const std::string input,
            const std::string out_file_path, const std::string callsign);

/**
 * @brief Encode raw binary data without a morse code callsign
 *
 * @param modulation
 * @param input Raw binary data to encode
 * @param input_length Number of bytes in the input buffer
 * @param out_file_path The path to the output WAV file to create
 * @return true if the WAV file was created successfully
 * @return false if the WAV file could not be created
 */
bool encode(const MWAVData::MODULATION modulation, const unsigned char *input,
            const int input_length, const std::string out_file_path);

}  // namespace MWAVData

namespace MWAVAprs {

bool encodeLocation(const std::string callsign,
                    const Aprs::Location location,
                    const std::string out_file_path);

}  // namespace MWAVAprs

#endif  // MWAV_H_