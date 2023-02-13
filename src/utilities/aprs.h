/**
 * @file aprs.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Header for APRS
 * @date 2023-02-11
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#ifndef APRS_H_
#define APRS_H_
#include <string>

#include "ax25.h"

class APRS {
 public:
  APRS();
  ~APRS();
  bool CreateAPRSMicEFrame(std::string callsign, uint8_t ssid, float latitude,
                           float longitude, float altitude, float speed,
                           float course);

  bool CreateAPRSPositionReport(std::string callsign, char symbol[2],
                                char time_code[2], uint8_t ssid, float latitude,
                                float longitude, int altitude, float speed,
                                float course);

  const AX25UiFrame& getFrame() const { return frame_; }

 private:
  bool setDestinationAddressField();
  bool setSourceAddressField(std::string callsign, uint8_t ssid);
  bool setAPRSInformationField(std::string information);
  std::vector<uint8_t> base91Encode(int value, unsigned int num_bytes);

  AX25UiFrame frame_ = {};
};

#endif  // APRS_H_