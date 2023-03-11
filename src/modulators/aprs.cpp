#include <cmath>
#include <iostream>
#include <vector>

#include "ax25.h"
#include "modulators.h"
#include "mwav-exception.h"
#include "mwav.h"

// class APRS {
//  public:
//   APRS(WavGen &wavgen) : wavgen_(wavgen) {}
//   ~APRS() {}
//
//   bool CreateAPRSPositionReport(std::string callsign, char symbol[2],
//                                 char time_code[2], uint8_t ssid, float
//                                 latitude, float longitude, int altitude,
//                                 float speed, float course);
//
//  private:
//   bool setDestinationAddressField();
//   bool setSourceAddressField(std::string callsign, uint8_t ssid);
//   std::vector<uint8_t> base91Encode(int value, unsigned int num_bytes);
//
//   AX25::Frame frame_;
//   WavGen &wavgen_;
// };

/*
bool APRS::CreateAPRSPositionReport(std::string callsign, char symbol[2],
                                    char time_code[6], uint8_t ssid,
                                    float latitude, float longitude,
                                    int altitude, float speed, float course) {



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
  // From spec:
  //  The comment may contain any printable ASCII characters (except | and ~,
  //  which are reserved for TNC channel switching).

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
    frame_.information.push_back(alt[i] << 1);  // Altitude
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

*/

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

std::vector<uint8_t> base91Encode(int value, unsigned int num_bytes) {
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

bool CheckPacketData(const mwav::AprsRequiredFields &packet) {
  if (packet.source_address.length() > 6 ||
      packet.source_address.length() < 3) {
    throw mwav::Exception("Invalid source address length");
  }

  if (packet.source_ssid > 15) {
    throw mwav::Exception("Invalid source ssid");
  }

  if (packet.destination_address.length() > 6 ||
      packet.destination_address.length() < 3) {
    throw mwav::Exception("Invalid destination address length");
  }

  if (packet.destination_ssid > 15) {
    throw mwav::Exception("Invalid destination ssid");
  }

  if (packet.symbol < '!' || packet.symbol > '~') {
    throw mwav::Exception("Invalid symbol");
  }

  return true;
}

bool CheckLocationData(const mwav::AprsLocationData &location) {
  // time code
  if (location.time_code.length() != 6) {
    throw mwav::Exception("Invalid time code length");
  }

  std::string hh = location.time_code.substr(0, 2);
  std::string mm = location.time_code.substr(2, 2);
  std::string ss = location.time_code.substr(4, 2);

  try {
    if (std::stoi(hh) < 0 || std::stoi(hh) > 23) {
      throw mwav::Exception("Invalid hour");
    }
    if (std::stoi(mm) < 0 || std::stoi(mm) > 59) {
      throw mwav::Exception("Invalid minute");
    }
    if (std::stoi(ss) < 0 || std::stoi(ss) > 59) {
      throw mwav::Exception("Invalid second");
    }
  } catch (std::invalid_argument &e) {
    throw mwav::Exception("Invalid time code");
  }

  if (location.latitude < -90 || location.latitude > 90) {
    throw mwav::Exception("Latitude out of range");
  }
  if (location.longitude < -180 || location.longitude > 180) {
    throw mwav::Exception("Longitude out of range");
  }
  // the dead sea to an arbitrarily high altitude
  if (location.altitude < -1400 || location.altitude > 150000) {
    throw mwav::Exception("Altitude out of range");
  }
  if (location.speed < 0 || location.speed > 400) {
    throw mwav::Exception("Speed out of range");
  }
  if (location.course < 0 || location.course > 360) {
    throw mwav::Exception("Course out of range");
  }

  return true;
}

bool AddRequiredFields(const mwav::AprsRequiredFields &required_fields,
                       AX25::Frame &frame) {
  try {
    if (!CheckPacketData(required_fields)) {
      return false;
    }
  } catch (mwav::Exception &e) {
    throw mwav::Exception("Invalid required fields: " + std::string(e.what()));
  }

  AX25::Address destination_address(required_fields.destination_address,
                                    required_fields.destination_ssid);
  AX25::Address source_address(required_fields.source_address,
                               required_fields.source_ssid, true);

  try {
    frame.AddAddress(destination_address);
    frame.AddAddress(source_address);
  } catch (mwav::Exception &e) {
    throw mwav::Exception("Invalid address: " + std::string(e.what()));
  }

  return true;
}

bool AddLocationData(const mwav::AprsLocationData &location,
                     const mwav::AprsRequiredFields &required_fields,
                     AX25::Frame &frame) {
  if (!CheckLocationData(location)) {
    return false;
  }

  std::vector<uint8_t> info;

  info.push_back('@');  // telemetry data

  // time code
  for (char c : location.time_code) {
    info.push_back(c);
  }
  info.push_back('z');  // UTC

  // Symbol Table ID
  if (required_fields.symbol_table == mwav::AprsSymbolTable::PRIMARY) {
    info.push_back('/');
  } else if (required_fields.symbol_table == mwav::AprsSymbolTable::SECONDARY) {
    info.push_back('\\');
  } else {
    throw mwav::Exception("Invalid symbol table");
  }

  // Latitude
  int uncompressed_lat = (int)(380926 * (90 - location.latitude));
  std::vector<uint8_t> compressed_lat = base91Encode(uncompressed_lat, 4);
  for (int i = 0; i < 4; i++) {
    info.push_back(compressed_lat[i]);  // YYYY
  }

  // Longitude
  int uncompressed_lon = (int)(190463 * (180 + location.longitude));
  std::vector<uint8_t> compressed_lon = base91Encode(uncompressed_lon, 4);
  for (int i = 0; i < 4; i++) {
    info.push_back(compressed_lon[i]);  // XXXX
  }

  // Symbol
  info.push_back(required_fields.symbol);

  // csT
  uint8_t c = (location.course / 4) + 33;
  info.push_back(c);

  // Speed
  const double speed_divisor = 0.076961;
  uint8_t s = 0;
  if (location.speed > 0) {
    s = (int)std::round(std::log(location.speed + 1) / speed_divisor);
  }
  info.push_back(s + 33); // s

  // Compression
  info.push_back((0b00111010 + 33));

  // Altitude
  int altitude = location.altitude;
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
    info.push_back(alt[i]);
  }

  for (char c : location.comment) {
    info.push_back(c);
  }

  frame.AddInformation(info);
  return true;
}

bool AddTelemetryData(const mwav::AprsTelemetryData &telemetry,
                      AX25::Frame &frame) {
  (void)telemetry;
  (void)frame;

  return true;
}

bool modulators::AprsEncodePacket(
    WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
    const mwav::AprsLocationData &location_data,
    const mwav::AprsTelemetryData &telemetry_data) {
  AX25::Frame frame;
  AddRequiredFields(required_fields, frame);
  if (required_fields.location_data) {
    AddLocationData(location_data, required_fields, frame);
  }
  if (required_fields.telemetry_data) {
    AddTelemetryData(telemetry_data, frame);
  }

  frame.BuildFrame();
  if (modulators::AfskEncodeBitStream(wavgen, frame.GetBitStream())) {
    return true;
  } else {
    return false;
  }
}