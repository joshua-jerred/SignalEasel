#include "mwav.h"

using namespace MWAVData;

int main() {
    std::string input = "Hello, world!";
    std::string out_file_path = "test.wav";
    std::string callsign = "KD9GDC";
    encode(MODE::ASCII, MODULATION::BPSK_125, input, out_file_path, callsign);
}