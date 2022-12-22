#include <iostream>
#include <string>

#include "psk.h"

/**
 * @brief Main function for when using as a command line utility.
 * @details
 * Usage: ./psk -m [mode] -s [symbol_rate] -f [filename] -t "text to encode"
 * or echo "test" | ./psk # uses defaults 
 */
int main(int argc, char** argv) {
    std::string message = "";
    std::string filename = "out.wav";
    PSK::Mode mode = PSK::BPSK;
    PSK::SymbolRate symbol_rate = PSK::S125;

    int message_flag = 0;

    for (int i = 0; i < argc; i++) {
        if (std::string(argv[i]) == "-m") {
            std::cout << "mode" << std::endl;
            if (std::string(argv[i + 1]) == "bpsk") {
                mode = PSK::BPSK;
            } else if (std::string(argv[i + 1]) == "qpsk") {
                mode = PSK::QPSK;
            } else {
                std::cout << "Invalid mode: -m bpsk | -m qpsk" << std::endl;
                return 1;
            }
        }
        if (std::string(argv[i]) == "-s") {
            if (std::string(argv[i + 1]) == "125") {
                symbol_rate = PSK::S125;
            } else if (std::string(argv[i + 1]) == "250") {
                symbol_rate = PSK::S250;
            } else if (std::string(argv[i + 1]) == "500") {
                symbol_rate = PSK::S500;
            } else if (std::string(argv[i + 1]) == "1000") {
                symbol_rate = PSK::S1000;
            } else {
                std::cout << "Invalid symbol rate: -s 125 | -s 250 | -s 500" << std::endl;
                return 1;
            }
        }
        if (std::string(argv[i]) == "-f") {
           filename = std::string(argv[i + 1]);
        }
        if (std::string(argv[i]) == "-t") {
            message = std::string(argv[i + 1]);
            message_flag = 1;
        }
    }

    if (!message_flag) {
        std::cout << "Enter message (Ctrl+D after newline to end): \n";
        std::string line;
        std::stringstream buffer;
        while (std::getline(std::cin, line)){
            buffer << line;
            buffer << "\n";
        }
        message = buffer.str();
    }

    if (message.size() < 2) {
        std::cout << "No message detected!" << std::endl;
        return 1;
    } else {
        std::cout << std::endl << "Encoding message: \n" << message << std::endl;
    }

    std::cout << "Mode: " << (mode == PSK::BPSK ? "BPSK" : "QPSK") << ", ";
    std::cout << "Symbol Rate: " << (symbol_rate == PSK::S125 ? "125" : (symbol_rate == PSK::S250 ? "250" : (symbol_rate == PSK::S500 ? "500" : "1000"))) << ", ";
    std::cout << "Filename: " << filename << std::endl;
    std::cout << "Generating audio file..." << std::endl;

    PSK psk(filename, mode, symbol_rate);
    psk.encodeTextData(message);
    return 0;
}