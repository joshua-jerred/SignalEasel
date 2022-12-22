# MWAV
Everything here is thoroughly ***UNTESTED***. Although BPSK at 125 Symbols/s follows the correct waveform the others struggle. All PSK31 spec modes
can reliably be decoded by Fldigi, but I would not reccomend broadcasting *anything* until you test functionality.

A C++ library for modulating text and raw binary input into WAV files. Supports BPSK, QPSK, and AFSK.

This is a combination of my [PSK31 modulation project](https://github.com/joshua-jerred/PSK31-Modulation-WAV-Generator) 
, my [AFSK modulation project](https://github.com/joshua-jerred/AFSK-Modulation-WAV-Generator), and my
[WAV file generation project](https://github.com/joshua-jerred/WavGen).

## Audio Examples
https://user-images.githubusercontent.com/19292194/209236564-4de990d9-40b4-4785-bb48-aa3a5e986aa6.mp4
https://user-images.githubusercontent.com/19292194/209236788-5939fdf8-e597-486e-9a19-2827b53d13bd.mp4



A lot of the code was shared between these and I'm currently working on combining them and creating reliable modulation for all modes.

## PSK Modulation
You can read [my attempt to document the origonal process of BPSK and QPSK modulation here](https://joshuajer.red/projects/psk-modulation.html)

## AFSK Modulation
Documentation from the origonal project:

Basic implementation of AFSK modulation according to AFSK1200 spec.
Supports both NRZI (0 is change in tone, 1 is no change) and Bell 202 encoding.

Takes in raw binary data (char*) and encodes it into AFSK audio into WAV format.

![image](https://user-images.githubusercontent.com/19292194/208214035-59ed7b38-1e53-47ea-89d1-ee65b8cab46a.png)

Raw binary data is read with minimodem perfectly.

![image](https://user-images.githubusercontent.com/19292194/208214105-9eeb9f8c-9d8f-4074-880f-1fed30ee4563.png)
![image](https://user-images.githubusercontent.com/19292194/208214122-c7f256f7-96ae-49f4-8a14-75158461b834.png)

Source of the formulas:
https://notblackmagic.com/bitsnpieces/afsk/


So far my biggest challenge is the fact that most documentation contradicts what I've found so far.
Even the AX.25 spec is incorrect in one spot, and it's the spec document.

## Helpful resources:
 - http://www.aprs.org/doc/APRS101.PDF
 - http://n1vg.net/packet/
 - https://notblackmagic.com/bitsnpieces/afsk/
 - https://www.ax25.net/AX25.2.2-Jul%2098-2.pdf

Currently unlicensed, will become part of https://github.com/joshua-jerred/Giraffe

## Quick Reference
Decoding with minimodem:
``minimodem --read -f afsk1200.wav 1200 --binary-raw 16``
