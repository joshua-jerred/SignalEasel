# Signal Easel

![Signal Easel Logo](project/logo.svg)

A C++ library for modulating and demodulating various digital modes. It is designed to be easy to use/integrate into other projects.

For detailed documentation, please see [https://signaleasel.joshuajer.red](https://signaleasel.joshuajer.red).

## Supported modes

- AFSK1200
  - Modulation and Demodulation
  - ASCII mode for sending text
- AX.25 UI Frames (AFSK1200 with CRC, bit stuffing, NRZI)
  - Modulation and Demodulation
- APRS 
  - Modulation and Demodulation
  - (Compressed Location, Telemetry, Messages with ACK/REJ, User Defined, Invalid Format)
- SSTV (Robot36, Optional callsign overlay)
  - Modulation only
- PSK31 (Fldigi Compatible BPSK/QPSK 125, 250, 500, 1000 baud)
  - Modulation only
  - ARRL Specification using Varicode and Convolutional Encoding, supports Fldigi
  - Also supports raw binary PSK without encoding
- Morse Code For adding optional station identification in all modes
  - Modulation only

***

## Building/Installing

***Important:*** SSTV requires that you have magick++ installed with the modules related to fonts and the image formats you want to use. This is ***optional***. If you want to use the other modes, then you do not need any 3rd party libraries. This library was developed on a Linux system but it may work on others.

Build:

```bash
# Install Prerequisites
apt install libpulse-dev -y

# Clone the repository with submodules (WavGen and SSTV-Image-Tools)
git clone --recurse-submodules  https://github.com/joshua-jerred/MWAV.git

# Create and enter the build directory
cd MWAV && mkdir build && cd build

# Configure
cmake -DSSTV_ENABLED=OFF .. # Disable SSTV if you don't have magick++ installed, default is ON

# Build
make SignalEasel

# Install
# Installation is currently not supported, the library is in this directory
# The header file is in the include directory
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