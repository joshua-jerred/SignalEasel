#include "gtest/gtest.h"

#include <SignalEasel/ax25.hpp>
#include <SignalEasel/exception.hpp>

using namespace signal_easel;

TEST(Ax25_Address, basicConstructor) {
  const std::string input_address = "KD9TST";
  uint8_t input_ssid = 0x02;
  ax25::Address address(input_address, input_ssid);

  EXPECT_EQ(address.getAddressString(), input_address);
  EXPECT_EQ(address.getSsid(), input_ssid);
  EXPECT_TRUE(address.isValid());
}

TEST(Ax25_Address, arrayConstructor_decodeAddress) {
  const std::string input_address = "KD9TST";
  uint8_t input_ssid = 0x02;
  ax25::Address::AddressArray input_address_array = {
      'K' << 1, 'D' << 1, '9' << 1,       'T' << 1,
      'S' << 1, 'T' << 1, input_ssid << 1};

  ax25::Address address(input_address_array);

  EXPECT_EQ(address.getAddressString(), input_address);
  EXPECT_EQ(address.getSsid(), input_ssid);
}

TEST(Ax25_Address, encodeAddress) {
  const std::string input_address = "KD9TST";
  uint8_t input_ssid = 0x02;
  bool input_is_last_address = true;
  ax25::Address address(input_address, input_ssid, input_is_last_address);

  auto out_address = address.encodeAddress();

  for (size_t i = 0; i < input_address.length(); i++) {
    EXPECT_EQ(out_address.at(i), input_address.at(i) << 1);
  }
  // last address bit
  EXPECT_NE(out_address.at(6) & 0b00000001, 0);
  // ssid
  EXPECT_EQ(out_address.at(6) & 0b00011110, input_ssid << 1);
}

TEST(Ax25_Address, checksAddressCharacters) {
  std::vector<std::string> valid_addresses = {"AAA", "ABA", "ABCD", "01234",
                                              "012345"};
  std::vector<std::string> invalid_addresses = {"KD9!!!", "AA", "AAAAAAA",
                                                "0123456"};

  for (auto address : valid_addresses) {
    EXPECT_NO_THROW(ax25::Address(address, 0x00)) << address;
  }
  for (auto address : invalid_addresses) {
    EXPECT_THROW(ax25::Address(address, 0x00), Exception) << address;
  }
}

TEST(Ax25_Address, padsWithSpaces) {
  const std::string input_address = "TEST";
  ax25::Address address(input_address, 0x00);

  EXPECT_EQ(address.getAddressString(), input_address);
  auto out_array = address.encodeAddress();
  for (size_t i = 0; i < input_address.length(); i++) {
    EXPECT_EQ(out_array.at(i), input_address.at(i) << 1);
  }
  EXPECT_EQ(out_array.at(4), ' ' << 1);
  EXPECT_EQ(out_array.at(5), ' ' << 1);
}

TEST(Ax25_Address, flag_bits_decode) {
  ax25::Address::AddressArray all_flags_set_array = {
      'K' << 1, 'D' << 1, '9' << 1, 'T' << 1, 'S' << 1, 'T' << 1, 0b11100001};
  ax25::Address::AddressArray all_flags_not_set_array = {
      'K' << 1, 'D' << 1, '9' << 1, 'T' << 1, 'S' << 1, 'T' << 1, 0b00000000};

  ax25::Address address_all_set(all_flags_set_array);
  ax25::Address address_all_not_set(all_flags_not_set_array);

  EXPECT_TRUE(address_all_set.isLastAddress());
  EXPECT_TRUE(address_all_set.isCommandOrResponse());
  EXPECT_TRUE(address_all_set.isReservedBit1Set());
  EXPECT_TRUE(address_all_set.isReservedBit2Set());

  EXPECT_FALSE(address_all_not_set.isLastAddress());
  EXPECT_FALSE(address_all_not_set.isCommandOrResponse());
  EXPECT_FALSE(address_all_not_set.isReservedBit1Set());
  EXPECT_FALSE(address_all_not_set.isReservedBit2Set());
}