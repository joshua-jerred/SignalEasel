#include "gtest/gtest.h"

#include <SignalEasel/psk.hpp>

using namespace signal_easel;

TEST(PSK_test, bpsk) {
  psk::Settings settings;
  settings.mode = psk::Settings::Mode::BPSK;
  settings.symbol_rate = psk::Settings::SymbolRate::SR_500;

  psk::Modulator modulator(settings);
  modulator.encodeString("Hello World!");
  modulator.encodeString("Hello World!");
  modulator.writeToFile("test_bpsk_125.wav");
}