
#include <iostream>
#include <bitset>
#include <iomanip>

#include "ax25.h"

int main() {
  std::cout << std::bitset<8>(0x55) << std::endl;
  std::cout << std::bitset<8>(AX25::reverse_bits(0x55)) << std::endl;


  AX25::Address address1("KK7EWJ", 0, false);
  AX25::Address address2("KD9GDC", 11, false);
  AX25::Address address3("WIDE2", 1, true);
  std::vector<uint8_t> information = {'T', 'e', 's', 't', '.'};

  AX25::Frame frame;
  frame.AddAddress(address1);
  frame.AddAddress(address2);
  frame.AddAddress(address3);
  frame.AddInformation(information);
  frame.BuildFrame();
  frame.Print();
  frame.PrintBitStream();

  return 0;
}