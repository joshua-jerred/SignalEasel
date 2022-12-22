/**
 * @file mwav.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief 
 * @date 2022-12-22
 * @copyright Copyright (c) 2022
 * @version 0.1
 */

#include "mwav.h"

bool addPSK(WavGen &wavgen, const std::string &message,
		       const MWAVData::MODULATION modulation);

bool MWAVData::encode(  // <-- Does not include a morse code callsign
		const MODULATION modulation, 
		const std::string input,
		const std::string out_file_path
		) {
	WavGen wavgen = WavGen(out_file_path);
	return addPSK(wavgen, input, modulation);
}

bool MWAVData::encode(  // <-- Includes a morse code callsign
		const MWAVData::MODULATION modulation, 
		const std::string input,
		const std::string out_file_path, 
		const std::string callsign) {
	WavGen wavgen = WavGen(out_file_path);

	if (!modulators::Morse(wavgen, callsign)) {  // Add the callsign
		return false;
	};

	wavgen.addSineWave(0, 0, 0.5);  // Add a 0.5 second pause
	
	return addPSK(wavgen, input, modulation);// Add the modulated data
}

/**
 * @brief Abstraction layer for PSK Modulation to prevent code duplication
 * and to prevent QPSK 1000.
 */
bool addPSK(
		WavGen &wavgen,
		const std::string &message,
		const MWAVData::MODULATION modulation) {

	modulators::PskSymbolRate symbol_rate;
	modulators::PskMode psk_mode;

	// Provides protection from invalid modulation (QPSK 1000)
	// This was built for Fldigi compatibility
	switch (modulation) 
	{
	case MWAVData::MODULATION::BPSK_125:
		symbol_rate = modulators::PskSymbolRate::S125;
		psk_mode = modulators::PskMode::BPSK;
		break;
	case MWAVData::MODULATION::BPSK_250:
		symbol_rate = modulators::PskSymbolRate::S250;
		psk_mode = modulators::PskMode::BPSK;
		break;
	case MWAVData::MODULATION::BPSK_500:
		symbol_rate = modulators::PskSymbolRate::S500;
		psk_mode = modulators::PskMode::BPSK;
		break;
	case MWAVData::MODULATION::BPSK_1000:
		symbol_rate = modulators::PskSymbolRate::S1000;
		psk_mode = modulators::PskMode::BPSK;
		break;
	case MWAVData::MODULATION::QPSK_125:
		symbol_rate = modulators::PskSymbolRate::S125;
		psk_mode = modulators::PskMode::QPSK;
		break;
	case MWAVData::MODULATION::QPSK_250:
		symbol_rate = modulators::PskSymbolRate::S250;
		psk_mode = modulators::PskMode::QPSK;
		break;
	case MWAVData::MODULATION::QPSK_500:
		symbol_rate = modulators::PskSymbolRate::S500;
		psk_mode = modulators::PskMode::QPSK;
		break;
	default:
		throw std::invalid_argument("psk_ascii() does not support this modulation");
		break;
	}

	return modulators::PskAscii(wavgen, message, psk_mode, symbol_rate);
}