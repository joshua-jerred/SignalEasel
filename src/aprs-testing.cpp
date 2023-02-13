#include "mwav.h"

int main() {
  std::string callsign = "TSTCAL";
  char symbol[2] = {'/', '>'};  // Dot = // , Balloon = /O, Car = />
  std::string time_code = "092345";
  uint8_t ssid = 0;
  float latitude = 49.5;
  float longitude = -72.75;
  int altitude = 1234;
  float speed = 36.2;  // knots
  int course = 88;     // course

  Aprs::Location loc = {
      .callsign = callsign,
      .ssid = ssid,
      .time_code = {time_code},
      .latitude = latitude,
      .longitude = longitude,
      .altitude = altitude,
      .speed = speed,
      .symbol_table = Aprs::Location::SymbolTable::PRIMARY,
      .symbol = symbol[0],
      .course = course,
  };

  MWAVAprs::encodeLocation("TSTCAL", loc, "aprs-test.wav");
}