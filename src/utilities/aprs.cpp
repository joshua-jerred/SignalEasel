/**
 * @file ax25.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Basic AX-25 Frame Encoding
 * @date 2023-02-11
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#include "aprs.h"
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


  /* Destination address:
    - The 6 latitude digits
    - A 3-bit message identifier (includes an emergency setting)
    - The North/South indicator
    - The West/East indicator
    - The Longitude offset indicator
    - The generic APRS digipeater path code

     The 7 bytes of the destination address are encoded as follows:
    | Lat Digit 1 + Message Bit A        | Lat Digit 2 + Message Bit B      |
    | Lat Digit 3 + Message Bit C        | Lat Digit 4 + N/S Lat Indicator  |
    | Lat Digit 5 + Lon Offset Indicator | Lat Digit 6 + W/E Long indicator |
    | APRS Digipeater Path Code          |

    Lat format: 33 25.64
    First Byte: 3 + 1
    '3' = 00110011 | 01000000
    00110011 | 01000000 = 's'
    's' = 01110011
  */
bool APRS::CreateAPRSMicEFrame(std::string callsign, uint8_t ssid,
                               float latitude, float longitude, float altitude,
                               float speed, float course) {
  const uint8_t message_type = 0b100; // [A, B, C] 0b000 = Emergency 
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

  bool lon_positive = longitude > 0; // East = true, West = false
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
  destination_address[4] = 
    ((int)(lat_minutes * 10) % 10 + 48) | (lon_offset ? 0b01000000 : 0b00000000);
  destination_address[5] = 
    ((int)(lat_minutes * 100) % 10 + 48) | (lon_positive ? 0b00000000 : 0b01000000);
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

  // Info field
  frame_.information.push_back('`' << 1);

  std::cout << frame_;
  return true;
}

int main() {
  std::string callsign = "TSTCAL";
  uint8_t ssid = 0; // VIA
  float latitude = 33.427333;
  float longitude = -11.939333;
  float altitude = 0;
  float speed = 0;
  float course = 0;

  APRS aprs;
  aprs.CreateAPRSMicEFrame(callsign, ssid, latitude, longitude, altitude, speed,
                           course);

  return 0;
}

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
