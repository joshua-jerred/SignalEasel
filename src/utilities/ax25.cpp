#include "ax25.h"
#include "bit-stream.h"

bool AX25UiFrame::encode(WavGen &wavgen) {
  // First verify that the frame is valid
  for (int i = 0; i < 7; i++) {  // Destination address
    if (destination_address[i] == 0x00) {
      return false;
    }
  }

  for (int i = 0; i < 7; i++) {  // Source address
    if (source_address[i] == 0x00) {
      return false;
    }
  }

  if (digipeater_addresses.size() > 56 ||
      digipeater_addresses.size() % 7 != 0) {  // Digipeater addresses
    return false;
  }

  if (information.size() == 0 || information.size() > 256) {  // Information
    return false;
  }

  calculateFcs();

  BitStream bit_stream;
  uint8_t byte_buffer[2];

  // Flag
  byte_buffer[0] = flag;
  bit_stream.addBits(byte_buffer, 8);

  // Destination address
  bit_stream.addBits(destination_address, 56);

  // Digipeater addresses
  for (unsigned int i = 0; i < digipeater_addresses.size(); i++) {
    byte_buffer[0] = digipeater_addresses[i];
    bit_stream.addBits(byte_buffer, 8);
  }

  // Source address
  bit_stream.addBits(source_address, 56);

  // Control
  byte_buffer[0] = control;
  bit_stream.addBits(byte_buffer, 8);

  // Protocol ID
  byte_buffer[0] = protocol_id;
  bit_stream.addBits(byte_buffer, 8);

  // Information
  for (unsigned int i = 0; i < information.size(); i++) {
    byte_buffer[0] = information[i];
    bit_stream.addBits(byte_buffer, 8);
  }



  bit_stream.pushBufferToBitStream();
  bit_stream.dumpBitStream();
  return true;
}

void AX25UiFrame::calculateFcs() {
  // Calculated between the start flag and the FCS field
}

/**
 * @brief Debugging function to print the AX25 frame
 * @param os
 * @param frame
 * @return std::ostream&
 */
std::ostream& operator<<(std::ostream& os, const AX25UiFrame& frame) {
  os << "AX25 Frame: " << std::endl;

  os << "0x" << std::hex << (int)frame.flag;
  std::cout << "|--|";
  for (int i = 0; i < 7; i++) {
    os << (char)(frame.destination_address[i] >> 1);
  }
  std::cout << "|--|";
  for (int i = 0; i < 7; i++) {
    os << (char)(frame.source_address[i] >> 1);
  }
  std::cout << "|--|";
  for (unsigned int i = 0; i < frame.digipeater_addresses.size(); i++) {
    os << (char)(frame.digipeater_addresses[i] >> 1);
  }
  std::cout << "|--|";
  os << "0x" << std::hex << (int)frame.control;
  std::cout << "|--|";
  os << "0x" << std::hex << (int)frame.protocol_id;
  std::cout << std::endl;
  for (unsigned int i = 0; i < frame.information.size(); i++) {
    os << (char)(frame.information[i] >> 1);
  }
  std::cout << std::endl;
  os << "0x" << std::hex << (int)frame.fcs[0] << " 0x" << std::hex
     << (int)frame.fcs[1];
  std::cout << "|--|";
  os << "0x" << std::hex << (int)frame.flag2 << std::endl;
  return os;
}