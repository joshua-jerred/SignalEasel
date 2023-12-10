#include "gtest/gtest.h"

#include <SignalEasel/ax25.hpp>

using namespace signal_easel;

TEST(Ax25_Frame, basicFrame) {
  ax25::Frame frame;
  ax25::Address dst_address("TEST", 0, false);
  ax25::Address src_address("TEST2", 0, true);
  dst_address.assertIsValid();
  src_address.assertIsValid();
  frame.setDestinationAddress(dst_address);
  frame.setSourceAddress(src_address);

  EXPECT_TRUE(frame.isFrameValid());
}