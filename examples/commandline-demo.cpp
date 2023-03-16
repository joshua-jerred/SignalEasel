/**
 * OUT OF DATE, NEEDS TO BE UPDATED
*/

#include "mwav.h"
#include <iostream>
#include <string>
#include <stdexcept>

#define OUTPUT_FILE "output.wav"

using namespace MWAVData;

int getIntInput () {
    std::string input;
    int inputInt;
    while (true) {
        std::cin >> input;
        try {
            inputInt = std::stoi(input);
            return inputInt;
        } catch (std::invalid_argument) {
            std::cout << "Invalid input, enter a number : " << std::endl;
        }
    }
}

std::string getStringInput () {
    std::string input, line;
    while (std::getline(std::cin >> std::ws, line)) {
        if (!line.empty()) {
            input += line;
        }
    }
    return input;
}

int main() {
    std::cout << "MWAV Modulation Library Command Line Example" << std::endl;
    
    int modulationType = 0;
    while (true) {
        std::cout << "Select a modulation type - [1] BPSK, [2] QPSK, [3] AFSK" << std::endl;
        std::cout << "1, 2, 3: ";
        modulationType = getIntInput();
        if (modulationType == 1 || modulationType == 2 || modulationType == 3) {
            break;
        }
    }

    std::cout << std::endl;

    int symbolRate = 0;
    if (modulationType == 1 || modulationType == 2) {
        while (true) {
            std::cout << "Select a symbol rate - [1] 125, [2] 250, [3] 500, [4] 1000" << std::endl;
            std::cout << "1, 2, 3, 4: ";
            symbolRate = getIntInput();
            if (symbolRate == 1 || symbolRate == 2 || symbolRate == 3 || symbolRate == 4) {
                if (symbolRate == 4 && modulationType == 2) {
                    std::cout << "! -- QPSK has a maximum symbol rate of 500 -- !" << std::endl;
                    continue;
                }
                break;
            }
        }
    }

    std::cout << std::endl;

    int callsign = 0;
    while (true) {
        std::cout << "Include a morse callsign? [1] Yes, [2] No" << std::endl;
        std::cout << "1, 2: ";
        callsign = getIntInput();
        if (callsign == 1 || callsign == 2) {
            break;
        }
    }

    std::cout << std::endl;

    std::string callsignString;
    if (callsign == 1) {
        std::cout << "Enter a callsign (between 4 and 6 characters): ";
        std::cin >> callsignString;
        if (callsignString.length() < 4 || callsignString.length() > 6) {
            std::cout << "Callsign must be between 4 and 6 characters" << std::endl;
            return 1;
        }
        std::cout << std::endl;
    }

    std::cout << "Enter a message to encode terminate with newline and CTRL+D: " << std::endl;
    std::string message = getStringInput();

    std::cout << std::endl;

    std::cout << "Encoding message...";

    MODULATION modulation;

    if (modulationType == 1) {
        switch (symbolRate) {
            case 1:
                modulation = MODULATION::BPSK_125;
                break;
            case 2:
                modulation = MODULATION::BPSK_250;
                break;
            case 3:
                modulation = MODULATION::BPSK_500;
                break;
            case 4:
                modulation = MODULATION::BPSK_1000;
                break;
        }
    } else if (modulationType == 2) {
        switch (symbolRate) {
            case 1:
                modulation = MODULATION::QPSK_125;
                break;
            case 2:
                modulation = MODULATION::QPSK_250;
                break;
            case 3:
                modulation = MODULATION::QPSK_500;
                break;
        }
    } else if (modulationType == 3) {
        modulation = MODULATION::AFSK1200;
    }

    if (callsign == 1) {
        if (!encode(modulation, message, OUTPUT_FILE, callsignString)) {
            std::cout << "Failed to encode message" << std::endl;
            return 1;
        }
    } else {
        if (!encode(modulation, message, OUTPUT_FILE)) {
            std::cout << "Failed to encode message" << std::endl;
            return 1;
        }
    }

    std::cout << "Done! " + std::string(OUTPUT_FILE) << std::endl; 

    return 0;
}


    /*
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
    */