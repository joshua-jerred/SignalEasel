#include "mwav.h"
#include <iostream>

using namespace MWAVData;

int main() {
    std::string input = 
    "Hello, world! This is a test of the MWAV Modulation library.";
    std::string callsign = "CA1lSN";
    // -------------------------------------------------
    // --------------------- ASCII ---------------------
    // -------------------------------------------------

    // --------------------- BPSK ----------------------
    // With Callsign
    if (!encode(MODULATION::BPSK_125, input, "call-bpsk-125.wav", callsign)) {
        std::cout << "Failed to encode BPSK 125 with callsign" << std::endl;
    }
    if (!encode(MODULATION::BPSK_250, input, "call-bpsk-250.wav", callsign)) {
        std::cout << "Failed to encode BPSK 250 with callsign" << std::endl;
    }
    if (!encode(MODULATION::BPSK_500, input, "call-bpsk-500.wav", callsign)) {
        std::cout << "Failed to encode BPSK 500 with callsign" << std::endl;
    }
    if (!encode(MODULATION::BPSK_1000, input, "call-bpsk-1000.wav", callsign)) {
        std::cout << "Failed to encode BPSK 1000 with callsign" << std::endl;
    }
    // Without Callsign
    if (!encode(MODULATION::BPSK_125, input, "bpsk-125.wav")) {
        std::cout << "Failed to encode BPSK 125 without callsign" << std::endl;
    }
    if (!encode(MODULATION::BPSK_250, input, "bpsk-250.wav")) {
        std::cout << "Failed to encode BPSK 250 without callsign" << std::endl;
    }
    if (!encode(MODULATION::BPSK_500, input, "bpsk-500.wav")) {
        std::cout << "Failed to encode BPSK 500 without callsign" << std::endl;
    }
    if (!encode(MODULATION::BPSK_1000, input, "bpsk-1000.wav")) {
        std::cout << "Failed to encode BPSK 1000 without callsign" << std::endl;
    }

    // --------------------- QPSK ----------------------
    // With Callsign
    if (!encode(MODULATION::QPSK_125, input, "call-qpsk-125.wav", callsign)) {
        std::cout << "Failed to encode QPSK 125 with callsign" << std::endl;
    }
    if (!encode(MODULATION::QPSK_250, input, "call-qpsk-250.wav", callsign)) {
        std::cout << "Failed to encode QPSK 250 with callsign" << std::endl;
    }
    if (!encode(MODULATION::QPSK_500, input, "call-qpsk-500.wav", callsign)) {
        std::cout << "Failed to encode QPSK 500 with callsign" << std::endl;
    }
    // Without Callsign
    if (!encode(MODULATION::QPSK_125, input, "qpsk-125.wav")) {
        std::cout << "Failed to encode QPSK 125 without callsign" << std::endl;
    }
    if (!encode(MODULATION::QPSK_250, input, "qpsk-250.wav")) {
        std::cout << "Failed to encode QPSK 250 without callsign" << std::endl;
    }
    if (!encode(MODULATION::QPSK_500, input, "qpsk-500.wav")) {
        std::cout << "Failed to encode QPSK 500 without callsign" << std::endl;
    }
    // --------------------- AFSK ----------------------
    if (!encode(MODULATION::AFSK1200, input, "afsk1200.wav")) {
        std::cout << "Failed to encode AFSK1200" << std::endl;
    }
}