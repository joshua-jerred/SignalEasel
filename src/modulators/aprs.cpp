#include <cmath>
#include <iostream>
#include <vector>

#include "ax25.h"
#include "modulators.h"

class APRS {
 public:
  APRS(WavGen &wavgen) : wavgen_(wavgen) {}
  ~APRS() {}

  bool encodeAX25Frame() {
    if (!frame_.encode(wavgen_)) {
      return false;
    }

    return true;
  }

  bool CreateAPRSPositionReport(std::string callsign, char symbol[2],
                                char time_code[2], uint8_t ssid, float latitude,
                                float longitude, int altitude, float speed,
                                float course);

  const AX25UiFrame &getFrame() const { return frame_; }

 private:
  bool setDestinationAddressField();
  bool setSourceAddressField(std::string callsign, uint8_t ssid);
  std::vector<uint8_t> base91Encode(int value, unsigned int num_bytes);

  WavGen &wavgen_;
  AX25UiFrame frame_ = {};
  int preamble_size_ = 0;
};

bool APRS::CreateAPRSPositionReport(std::string callsign, char symbol[2],
                                    char time_code[6], uint8_t ssid,
                                    float latitude, float longitude,
                                    int altitude, float speed, float course) {
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

bool modulators::APRSLocation(WavGen &wavgen, const Aprs::Location &location) {
  APRS aprs = APRS(wavgen);

  if (location.time_code.size() != 6) {
    return false;
  }
  char time_code[6];
  for (int i = 0; i < 6; i++) {
    if (location.time_code[i] < '0' || location.time_code[i] > '9') {
      return false;
    }
    time_code[i] = location.time_code[i];
  }

  char sym[2];
  sym[0] = location.symbol_table == Aprs::Location::SymbolTable::PRIMARY ? '/'
                                                                         : '\\';
  sym[1] = location.symbol;

  if (!aprs.CreateAPRSPositionReport(location.callsign, sym, time_code,
                                     location.ssid, location.latitude,
                                     location.longitude, location.altitude,
                                     location.speed, location.course)) {
    return false;
  }
  // Modulate the AX.25 frame
  if (!aprs.encodeAX25Frame()) {
    return false;
  }
  return true;
}