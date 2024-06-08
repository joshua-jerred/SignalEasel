#include "gtest/gtest.h"

#include <SignalEasel/sstv.hpp>

using namespace signal_easel;

TEST(SSTV_test, generateAudio) {
  sstv::Settings settings;
  settings.call_sign = "TEST";
  settings.overlay_call_sign = true;

  sstv::Modulator modulator(settings);
  modulator.encodeImage("test_image.png");
  modulator.writeToFile("test_image.wav");
}