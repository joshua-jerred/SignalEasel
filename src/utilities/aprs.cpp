/**
 * @file ax25.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Basic AX-25 Frame Encoding
 * @date 2023-02-11
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#include "aprs.h"

#include <cmath>
#include <iostream>

/*
APLIGA,WIDE2-1,qAR,KE0NHQ-3:/031410h4238.13N/09049.05WO092/024/A=011473 058TxC
 -2.40C  658.57hPa  7.01V 12S http://www.lightaprs.com

 2023-02-11 18:28:00 MST: EA4RCM>APRS,TCPIP*,qAC,T2EUSKADI:;
 EA4RCM-11*012815h4033.64N/00337.54WO311/038/A=011210U0230267|
 Radiosonda AEMET RS41 EA4RCM | T=-6.8C|Dir:311.6<0xc2><0xb0>|
 vVert:-5.2 m/s|-00005
*/
APRS::APRS() {}

APRS::~APRS() {}

bool APRS::CreateAPRSMicEFrame(std::string callsign, uint8_t ssid,
                               float latitude, float longitude, float altitude,
                               float speed, float course) {
  const uint8_t message_type = 0b100;  // [A, B, C] 0b000 = Emergency
  uint8_t destination_address[7];

  (void)ssid;
  (void)longitude;
  (void)altitude;
  (void)speed;
  (void)course;

  if (callsign.length() > 6 || callsign.length() < 4) {
    return false;
  }

  if (latitude > 90 || latitude < -90) {
    return false;
  }

  bool lat_positive = latitude > 0;
  if (!lat_positive) {
    latitude = -latitude;
  }

  bool lon_positive = longitude > 0;  // East = true, West = false
  if (!lon_positive) {
    longitude = -longitude;
  }

  bool lon_offset = longitude > 100;

  // Convert Latitude from DD.DDDDD to DD MM.MMMM
  int lat_degrees = (int)latitude;
  float lat_minutes = (latitude - lat_degrees) * 60;

  destination_address[0] =
      ((lat_degrees / 10) + 48) | ((message_type & 0b100) << 4);
  destination_address[1] =
      ((lat_degrees % 10) + 48) | ((message_type & 0b010) << 5);
  destination_address[2] =
      ((int)lat_minutes / 10 + 48) | ((message_type & 0b001) << 6);
  destination_address[3] =
      ((int)lat_minutes % 10 + 48) | (lat_positive ? 0b01000000 : 0b00000000);
  destination_address[4] = ((int)(lat_minutes * 10) % 10 + 48) |
                           (lon_offset ? 0b01000000 : 0b00000000);
  destination_address[5] = ((int)(lat_minutes * 100) % 10 + 48) |
                           (lon_positive ? 0b00000000 : 0b01000000);
  destination_address[6] = 0b00000000;

  frame_.destination_address[0] = destination_address[0] << 1;
  frame_.destination_address[1] = destination_address[1] << 1;
  frame_.destination_address[2] = destination_address[2] << 1;
  frame_.destination_address[3] = destination_address[3] << 1;
  frame_.destination_address[4] = destination_address[4] << 1;
  frame_.destination_address[5] = destination_address[5] << 1;
  // 6 already in place

  // Source address (callsign/ssid)
  frame_.source_address[0] = callsign[0] << 1;
  frame_.source_address[1] = callsign[1] << 1;
  frame_.source_address[2] = callsign[2] << 1;
  frame_.source_address[3] = callsign[3] << 1;
  frame_.source_address[4] = callsign[4] << 1;
  frame_.source_address[5] = callsign[5] << 1;
  frame_.source_address[6] = '-' << 1;
  frame_.source_address[7] = ssid << 1;

  /*
  Information field:
  [Byte][info]
  [0] Date Type ID []
    ` = Mic-E Data (0x1c?)

  [1] lon d+28 0–179 degrees.
  [2] lon m+28
  [3] lon h+28

  [4] speed+28
  [5] DC+28
  [6] SE+28
  [7] Symbol Code
  [8] Symbol Table ID
  [n] Mic-E Telemetry Data and Mic-E Status Text
  */
  frame_.information.push_back('`' << 1);

  std::cout << frame_;
  return true;
}

bool APRS::CreateAPRSPositionReport(std::string callsign, char symbol[2],
                                    char time_code[7], uint8_t ssid,
                                    float latitude, float longitude,
                                    int altitude, float speed, float course) {
  (void)ssid;
  (void)longitude;
  (void)altitude;
  (void)speed;
  (void)course;
  (void)symbol;

  if (callsign.length() > 6 || callsign.length() < 4) {
    return false;
  }

  if (latitude > 90 || latitude < -90) {
    return false;
  }

  // Destination address
  frame_.destination_address[0] = 'G' << 1;
  frame_.destination_address[1] = 'P' << 1;
  frame_.destination_address[2] = 'S' << 1;
  frame_.destination_address[3] = ' ' << 1;
  frame_.destination_address[4] = ' ' << 1;
  frame_.destination_address[5] = ' ' << 1;
  frame_.destination_address[6] = '0' << 1;

  // Source address (callsign/ssid)
  frame_.source_address[0] = callsign[0] << 1;
  frame_.source_address[1] = callsign[1] << 1;
  frame_.source_address[2] = callsign[2] << 1;
  frame_.source_address[3] = callsign[3] << 1;
  frame_.source_address[4] = callsign[4] << 1;
  frame_.source_address[5] = callsign[5] << 1;
  frame_.source_address[6] = '-' << 1;
  frame_.source_address[7] = ssid << 1;

  // Information field --------------------------------------------
  frame_.information.push_back('@' << 1);  // Position, timestamp, and comment

  // Timestamp
  for (int i = 0; i < 6; i++) {
    frame_.information.push_back(time_code[i] << 1);
  }
  frame_.information.push_back('z' << 1);  // UTC

  // Symbol Table ID
  frame_.information.push_back(symbol[0] << 1);

  // Latitude
  int uncompressed_lat = (int)(380926 * (90 - latitude));
  std::vector<uint8_t> compressed_lat = base91Encode(uncompressed_lat, 4);
  for (int i = 0; i < 4; i++) {
    frame_.information.push_back(compressed_lat[i] << 1);  // YYYY
  }

  // Longitude
  int uncompressed_lon = (int)(190463 * (180 + longitude));
  std::vector<uint8_t> compressed_lon = base91Encode(uncompressed_lon, 4);
  for (int i = 0; i < 4; i++) {
    frame_.information.push_back(compressed_lon[i] << 1);  // XXXX
  }

  // Symbol Code
  frame_.information.push_back(symbol[1] << 1);  // $

  // Next 3 bytes are csT
  // Course
  uint8_t c = (course / 4) + 33;
  frame_.information.push_back(c << 1);  // c

  // Speed
  const double speed_divisor = 0.076961;
  uint8_t s = 0;
  if (speed > 0) {
    s = (int)std::round(std::log(speed + 1) / speed_divisor);
  }
  frame_.information.push_back((s + 33) << 1);  // s

  // Comp Type nnGNNCCC
  // nn = not used
  // G = GPS Fix (1 = current, 0 = old)
  // NN = NMEA Source (00 = other)
  // CCC = Compression Origin (010 = Software)
  // 00100010
  frame_.information.push_back((0b00111010 + 33) << 1);

  // Comment (Max 40 chars)
  /* From spec:
    The comment may contain any printable ASCII characters (except | and ~,
    which are reserved for TNC channel switching).*/

  // Altitude (Chap 6), Chapter 12, chapter 16 (status)
  // Altitude '/A=aaaaaa' in feet 001234 = 1234 feet
  char alt[9] = {'/', 'A', '=', '0', '0', '0', '0', '0', '0'};
  if (altitude > 0 && altitude < 999999) {
    int i = 8;
    while (altitude > 0) {
      alt[i] = (altitude % 10) + '0';
      altitude /= 10;
      i--;
    }
  }

  for (int i = 0; i < 9; i++) {
    frame_.information.push_back(alt[i] << 1); // Altitude
  }

  return true;
}

std::vector<uint8_t> APRS::base91Encode(int value, unsigned int num_bytes) {
  std::vector<uint8_t> encoded;

  for (int i = num_bytes - 1; i > -1; i--) {
    char c = value / (int)(std::pow(91, i));
    value = value % (int)(std::pow(91, i));
    encoded.push_back(c + 33);
  }

  if (encoded.size() != num_bytes) {
    throw std::runtime_error("Encoded value is not the correct size");
  }

  return encoded;
}

/*
int main() {
  std::string callsign = "TSTCAL";
  char symbol[2] = {'/', '>'};  // Dot = // , Balloon = /O, Car = />
  char time_code[7] = "092345";
  char ssid = '0';
  float latitude = 49.5;
  float longitude = -72.75;
  int altitude = 1234;
  float speed = 36.2;  // knots
  float course = 88;   // course

  APRS aprs;
  aprs.CreateAPRSPositionReport(callsign, symbol, time_code, ssid, latitude,
                                longitude, altitude, speed, course);

  std::cout << aprs.getFrame();

  return 0;
}
*/

/*
bool APRS::setSourceAddressField(std::string callsign, uint8_t ssid) {
  if (callsign.length() > 6 || callsign.length() < 4) {
    return false;
  }

  uint8_t source_address[7];

  // Encode callsign
  int i = 0;
  for (int i = 0; i < callsign.length(); i++) {
    source_address[i] = callsign[i] << 1;
  }

  // Encode SSID
}
*/
