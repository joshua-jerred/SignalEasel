# SignalEasel

SignalEasel is a C++ library for audio modulation and demodulation through a
variety of analog and digital modes via PulseAudio and `.wav` files. It also
provides encoding and decoding for higher level protocols.

![SignalEasel](docs/images/logo.svg)

[Detailed Documentation](https://signaleasel.joshuajer.red)

[Repository](https://github.com/joshua-jerred/SignalEasel)

## Features
- APRS
  - Full encoding and decoding of APRS packets through continuous and discrete modes
  - Location (Compressed)
  - Telemetry Data Reports w/Parameter Metadata Messages
  - Messages with ACK/REJ
  - User Defined Packets
- AX.25
  - Encoding and Decoding of UI Frames
  - NRZI, CRC-16, Bit Stuffing
- AFSK
  - AFSK1200 (Bell 202)
  - ASCII mode for sending text
- BPSK & QPSK
  - Varicode and Convolutional Encoding (ARRL PSK31 Spec)
  - Fldigi mode at 125, 250, 500, 1000 baud
  - Also supports raw binary PSK without encoding
- Morse Code for additional station identification
- SSTV (Robot36, Optional Call Sign & data overlay)
  - Modulation only

***

## Build / Install

### Optional Features & Dependencies

There are a few optional features that require additional dependencies. These
features can be toggled in the root CMakeLists.txt

```
option(USE_PULSEAUDIO ... ON)
option(SSTV_ENABLED ... OFF)
option(UNIT_TESTS ... OFF)
```

#### Real-Time Modulation/Demodulation - PulseAudio
PulseAudio is required for real-time modulation and demodulation. If enabled,
in the root CMakeLists.txt, SignalEasel will use PulseAudio for audio I/O.
Without PulseAudio, you can still use the library to generate and read `.wav`
files.

```bash
sudo apt-get install libpulse-dev
```

#### SSTV - ImageMagick++
SSTV requires that you have magick++ installed with the modules related to fonts and the image formats you want to use. This is ***optional***. If you want to use the other modes, then you do not need any 3rd party libraries. This library was developed on a Linux system but it may work on others.

Installation instructions: https://imagemagick.org/Magick++/Install.html

### Method 1: CMake
This library is designed to be used as a part of another CMake project. To use
it, you simply add the subdirectory to your project and link against it.

```cmake
add_subdirectory(lib/SignalEasel)

target_link_libraries(your_target SignalEasel)
```

### Method 2: Manual Installation
Not recommended, but it's as simple as building the static library and
including the `include` directory in your project.

```bash
# Clone the repository with submodules (WavGen and SSTV-Image-Tools)
git clone --recurse-submodules  https://github.com/joshua-jerred/SignalEasel

# Create and enter the build directory
cd SignalEasel && mkdir build && cd build

# Configure
cmake ..

# Build
make SignalEasel # Builds a static library at `build/libSignalEasel.a`
```

***

## Building/Installing Magick++

```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libjpeg62-dev libpng-dev libfreetype6-dev

git clone https://github.com/ImageMagick/ImageMagick.git ImageMagick-7.1.0
cd ImageMagick-7.1.0
./configure --with-modules
make -j8
sudo make install
sudo ldconfig # May need to do more for linking
```

***

## Resources

- https://user.engineering.uiowa.edu/~eedesign/algorithm.pdf
- https://archive.org/details/dcc-2014-amateur-bell-202-modem-w-6-kwf-and-bridget-benson
- https://github.com/PhirePhly/aprs_notes