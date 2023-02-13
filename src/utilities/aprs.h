#ifndef APRS_H_
#define APRS_H_

namespace Aprs {
struct Location {
  enum class SymbolTable { PRIMARY, SECONDARY };

  std::string callsign;  // 3 - 6 characters
  uint8_t ssid;           // 0 - 15
  std::string time_code;  // hhmmss

  float latitude;   // Decimal degrees
  float longitude;  // Decimal degrees
  int altitude;     // Feet 0 - 99999
  float speed;      // Knots 0 - 400

  SymbolTable symbol_table;  // Symbol table
  char symbol;               // Symbol character
  int course;                // Degrees 0 - 359
};
}  // namespace Aprs

#endif // APRS_H_