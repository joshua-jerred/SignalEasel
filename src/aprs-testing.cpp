#include "mwav.h"
#include <iostream>
#include "ax25.h"

int main() {


  AX25UiFrame frame;
  //frame.destination_address = {0x41, 0x19, 0x19, 0x02, 0x02, 0x02, 0x07};
  frame.destination_address[0] = 0x41;
  frame.destination_address[1] = 0x19;
  frame.destination_address[2] = 0x19;
  frame.destination_address[3] = 0x02;
  frame.destination_address[4] = 0x02;
  frame.destination_address[5] = 0x02;
  frame.destination_address[6] = 0x07;
  // 0x69 0x11 0x4E 0x71 0x11 0x61 0x86
  frame.source_address[0] = 0x69;
  frame.source_address[1] = 0x11;
  frame.source_address[2] = 0x4E;
  frame.source_address[3] = 0x71;
  frame.source_address[4] = 0x11;
  frame.source_address[5] = 0x61;
  frame.source_address[6] = 0x86;

  
  std::string info = ":ALL               Hello World!";
  for (char c : info) {
    frame.information.push_back(c);
  }

  WavGen wavgen("this-is-a-test.wav");
  frame.encode(wavgen);
  
  /*
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
  */
}