/**
 * @file data_modulation.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Public interface for data modulators.
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#ifndef MWAV_DATA_MODULATION_HPP_
#define MWAV_DATA_MODULATION_HPP_

#include <string>

namespace mwav::data {
/**
 * @brief The modulation modes for data encoding.
 */
enum class Mode {
  BPSK_125,  /**< BPSK 125 baud */
  BPSK_250,  /**< BPSK 250 baud */
  BPSK_500,  /**< BPSK 500 baud */
  BPSK_1000, /**< BPSK 1000 baud */
  QPSK_125,  /**< QPSK 125 baud */
  QPSK_250,  /**< QPSK 250 baud */
  QPSK_500,  /**< QPSK 500 baud */
  AFSK1200   /**< AFSK 1200 baud */
};

/**
 * @brief Encode a string of data into BPSK/QPSK/AFSK1200 audio, with or without
 * a morse code callsign.
 *
 * @details This function will encode a string of data into a wav file depending
 * on the modulation type. The input string must be ASCII characters only. This
 * will generate a wav file at the specified path.
 *
 * If an optional callsign is specified then it will be encoded with morse code
 * at the end of the transmission. This is highly recommended, and possibly
 * legally required, if you are transmitting on ham radio frequencies.
 *
 * If the modulation mode is BPSK/QPSK then it will be encoded with the
 * PSK31 standard (varicode and convolutional coding). This means that it
 * can be decoded with Fldigi or other software that supports PSK31.
 *
 * With AFSK1200 it will *not* be encoded with AX.25 or NRZI, simply just
 * raw ASCII with 1200hz tones for 0 and 2200hz tones for 1. This will
 * support minimodem and other software that supports Bell 202 style AFSK.
 *
 * @param modulation The modulation mode to use.
 * @param input A string of data to encode, ASCII only.
 * @param out_file_path The path to the output wav file.
 * @param callsign *Optional* The callsign to use, must be 3-6 characters or
 * left blank.
 * @exception mwav::Exception
 */
void encodeString(Mode modulation_mode, const std::string &input,
                  const std::string &out_file_path,
                  const std::string &call_sign = "");

/**
 * @brief Encode a raw binary array of data into BPSK/QPSK/AFSK1200 audio with
 * a morse code call sign at the start.
 *
 * @details This function will take a raw binary array of data and encode it.
 * It will not be converted to ASCII. With BPSK/QPSK it will not be to the PSK31
 * spec, it will be raw binary. The same goes for AFSK1200.
 *
 * If an optional call sign is specified then it will be encoded with morse code
 * at the end of the transmission. This is highly recommended, and probably
 * legally required, if you are transmitting on ham radio frequencies.
 *
 * A call sign is *highly* recommended with these modes for legal identification
 * if transmitting due to this being off spec.
 *
 * @param modulation The modulation mode to use.
 * @param input Raw binary data to encode in the form of unsigned bytes.
 * @param input_length The number of bytes in the input array.
 * @param out_file_path The path to the output wav file.
 * @param call_sign The callsign to use, must be 3-6 characters.
 * @return true - Success
 * @return false - Failure
 * @exception mwav::Exception
 */
bool encodeRawData(const Mode modulation_mode, const unsigned char *input,
                   const int input_length, const std::string out_file_path,
                   const std::string callsign = "");

} // namespace mwav::data

#endif /* MWAV_DATA_MODULATION_HPP_ */