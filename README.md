# MWAV
A C++ library for generating WAV files for various digital modes including PSK31, AFSK1200, AX.25, APRS, and SSTV.

This is currently a work in progress, but the modes listed below are fully functional. Everything is subject to change. It uses my [WAV file generation library](https://github.com/joshua-jerred/WavGen) and my [SSTV Image Manipulation Tool](https://github.com/joshua-jerred/SSTV-Image-Tools).

## Supported modes
- APRS (Compressed Location, Telemetry, Messages with ACK/REJ, User Defined, Invalid Format)
- SSTV (Robot36, Optional callsign overlay)
- AX.25 UI Frames (AFSK1200 with CRC, bit stuffing, NRZI)
- PSK31 (Fldigi Compatible BPSK/QPSK 125, 250, 500, 1000 baud)
    - ARRL Specification using Varicode and Convolutional Encoding, supports Fldigi
    - Also supports raw binary PSK without encoding
- AFSK1200 (Raw binary and ASCII)
- Morse Code For adding optional station identification in all modes

***
## Usage
Everything is contained in the `mwav` namespace and header file.
``#include "mwav.h"``

The following are *bare minimum* examples. For more detailed examples, see the `examples` directory.

### PSK31
```cpp
mwav::EncodeString(mwav::DataModulation::BPSK_125, "Hello World!", "out-file.wav");
```

### SSTV
```cpp
mwav::EncodeSSTV("out-file.wav", "input-image.png");
```

### APRS
```cpp
/*
KD9GDC-11>APRS:@092345z/;#2b4+slO7P[/A=088132Hello World!
*/

mwav::AprsRequiredFields required;
required.source_address = "KD9GDC"; // 3-6 characters
required.source_ssid = 11; // Defaults to 0
required.destination_address = "APRS";
required.symbol_table = mwav::AprsSymbolTable::PRIMARY; // Default
required.symbol = 'O'; // Default is 'dot' or '/'

mwav::aprs_packet::CompressedPositionReport location;
location.time_code = "092345"; // hhmmss UTC
location.latitude = 38.51750;
location.longitude = -104.35173;
location.altitude = 88132; // in feet
location.speed = 125; // in knots
location.course = 180; // in degrees
location.comment = "Hello World!";

mwav::EncodeAprs("out-file.wav", required, location);
```

***
## Building/Installing

***Important:*** SSTV requires that you have magick++ installed with the modules related to fonts and the image formats you want to use. This is ***optional***. If you want to use the other modes, then you do not need any 3rd party libraries. This library was developed on a Linux system but it may work on others.

Build:
```bash
# Clone the repository with submodules (WavGen and SSTV-Image-Tools)
git clone --recurse-submodules  https://github.com/joshua-jerred/MWAV.git

# Create and enter the build directory
cd MWAV && mkdir build && cd build

# Configure
cmake -DSSTV_ENABLED=OFF .. # Disable SSTV if you don't have magick++ installed, default is ON

# Build
make MWAVlib

# Install
# Installation is currently not supported, the library is in this directory
# The header file is in the include directory
```

***

More Details from the old README

This is a combination of my [PSK31 modulation project](https://github.com/joshua-jerred/PSK31-Modulation-WAV-Generator) 
, my [AFSK modulation project](https://github.com/joshua-jerred/AFSK-Modulation-WAV-Generator), and my
[WAV file generation project](https://github.com/joshua-jerred/WavGen).

## Audio Examples
https://user-images.githubusercontent.com/19292194/209236564-4de990d9-40b4-4785-bb48-aa3a5e986aa6.mp4

https://user-images.githubusercontent.com/19292194/209236788-5939fdf8-e597-486e-9a19-2827b53d13bd.mp4



A lot of the code was shared between these and I'm currently working on combining them and creating reliable modulation for all modes.

## PSK Modulation
You can read [my attempt to document the original process of BPSK and QPSK modulation here](https://joshuajer.red/projects/psk-modulation.html)

## AFSK Modulation
Documentation from the original project:

Basic implementation of AFSK modulation according to AFSK1200 spec.
Supports both NRZI (0 is change in tone, 1 is no change) and Bell 202 encoding.

Takes in raw binary data (char*) and encodes it into AFSK audio into WAV format.

![image](https://user-images.githubusercontent.com/19292194/208214035-59ed7b38-1e53-47ea-89d1-ee65b8cab46a.png)

Raw binary data is read with minimodem perfectly.

![image](https://user-images.githubusercontent.com/19292194/208214105-9eeb9f8c-9d8f-4074-880f-1fed30ee4563.png)
![image](https://user-images.githubusercontent.com/19292194/208214122-c7f256f7-96ae-49f4-8a14-75158461b834.png)

Source of the formulas:
https://notblackmagic.com/bitsnpieces/afsk/


## Helpful resources:
 - http://www.aprs.org/doc/APRS101.PDF
 - http://n1vg.net/packet/
 - https://notblackmagic.com/bitsnpieces/afsk/
 - https://www.ax25.net/AX25.2.2-Jul%2098-2.pdf

Currently unlicensed, will become part of https://github.com/joshua-jerred/Giraffe
