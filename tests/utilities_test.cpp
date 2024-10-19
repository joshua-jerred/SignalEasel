/// =*============================= SignalEasel ==============================*=
/// A C++ library for audio modulation/demodulation into analog & digital modes.
/// Detailed documentation can be found here: https://signaleasel.joshuajer.red
///
/// @author Joshua Jerred
/// @date   2024-10-18
///
/// @copyright Copyright 2024 Joshua Jerred. All rights reserved.
/// @license   This project is licensed under the GNU GPL v3.0 license.
/// =*========================================================================*=

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "src/utilities.hpp"

TEST(utilities_test, isCallSignValid) {
  // -- Test Valid Call Signs --
  // From https://en.wikipedia.org/wiki/Amateur_radio_call_signs
  std::vector<std::string> valid_call_signs = {
      "K4X", "B2AA", "N2ASD", "A22A", "I20000X", "4X4AAA", "3DA0RS", "HL1AA"};

  for (const auto &call_sign : valid_call_signs) {
    EXPECT_TRUE(signal_easel::isCallSignValid(call_sign))
        << call_sign << " should be considered valid";
  }

  // -- Test Invalid Call Signs --
  std::vector<std::string> invalid_call_signs = {"K4", "BAA", "TOOLONGOFCALL",
                                                 "NON-ALPHANUMERIC", "1234"};

  for (const auto &call_sign : invalid_call_signs) {
    EXPECT_FALSE(signal_easel::isCallSignValid(call_sign))
        << call_sign << " should be considered invalid";
  }
}