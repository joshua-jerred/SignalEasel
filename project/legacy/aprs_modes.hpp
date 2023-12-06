/**
 * @file aprs_modulation.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Public interface for aprs modulation.
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#ifndef MWAV_APRS_MODULATION_HPP_
#define MWAV_APRS_MODULATION_HPP_

// Compressed Position Report (GPS)
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::CompressedPositionReport location,
                const std::string morse_callsign = "NOCALLSIGN");

// Telemetry (Data, Parameter names, Units, Coefficients, or Bitfields/Proj
// Name)
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::Telemetry telemetry,
                const aprs_packet::TelemetryPacketType packet_type,
                const std::string morse_callsign = "NOCALLSIGN");

// Message
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::Message message,
                const std::string morse_callsign = "NOCALLSIGN");

// Message ACK
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::MessageAck message_ack,
                const std::string morse_callsign = "NOCALLSIGN");

// Message NACK
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::MessageNack message_nack,
                const std::string morse_callsign = "NOCALLSIGN");

// User Defined APRS Packet
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::UserDefined user_defined_packet,
                const std::string morse_callsign = "NOCALLSIGN");

// Experimental
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::Experimental experimental_packet,
                const std::string morse_callsign = "NOCALLSIGN");

// Invalid
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::Invalid invalid_packet,
                const std::string morse_callsign = "NOCALLSIGN");

#endif /* MWAV_APRS_MODULATION_HPP_ */