#include "gtest/gtest.h"

#include <SignalEasel/psk.hpp>

using namespace signal_easel;

TEST(PSK_test, bpsk125) {
  psk::Settings settings;
  settings.mode = psk::Settings::Mode::BPSK;
  settings.symbol_rate = psk::Settings::SymbolRate::SR_125;

  psk::Modulator modulator(settings);
  modulator.encodeString("Hello World!");
  modulator.encodeString("Hello World!");
  modulator.writeToFile("test_bpsk_125.wav");
}

TEST(PSK_test, qpsk500) {
  psk::Settings settings;
  settings.mode = psk::Settings::Mode::QPSK;
  settings.symbol_rate = psk::Settings::SymbolRate::SR_500;

  psk::Modulator modulator(settings);
  modulator.encodeString("Hello World!");
  modulator.encodeString("Hello World!");
  modulator.writeToFile("test_qpsk_500.wav");
}

TEST(PSK_test, addCallSign) {
  psk::Settings settings;
  settings.mode = psk::Settings::Mode::BPSK;
  settings.symbol_rate = psk::Settings::SymbolRate::SR_125;
  settings.call_sign = "T3ST";
  settings.call_sign_mode =
      signal_easel::Settings::CallSignMode::BEFORE_AND_AFTER;

  psk::Modulator modulator(settings);
  modulator.encodeString("Hello World!");
  modulator.encodeString("Hello World!");
  modulator.writeToFile("test_psk_add_call_sign.wav");
}