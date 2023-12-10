#include "gtest/gtest.h"

#include <SignalEasel/ax25.hpp>

using namespace signal_easel;

TEST(Ax25_CRC, fcsCalcTest) {
  std::vector<uint8_t> input_data = {'T', 'E', 'S', 'T'};
  uint16_t expected_crc = 0x89D1;
  EXPECT_EQ(ax25::calculateFcs(input_data), expected_crc);
}