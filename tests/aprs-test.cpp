#include <iostream>

#include "mwav.h"
#include "wavgen.h"

int main() {
  mwav::AprsRequiredFields required_fields;
  mwav::AprsLocationData location_data;
  mwav::AprsTelemetryData telemetry_data;
  
  required_fields.source_address = "KD9GDC";
  required_fields.source_ssid = 11;
  required_fields.symbol = 'O';
  required_fields.location_data = true;
  required_fields.telemetry_data = false;

  location_data.time_code = "092345";
  location_data.latitude = 49.5;
  location_data.longitude = -72.75;
  location_data.altitude = 1234;
  location_data.speed = 36.2;
  location_data.course = 88;
  location_data.comment = "Test comment";
  
  mwav::EncodeAprs(required_fields, "aprs-test.wav", location_data, telemetry_data);
}