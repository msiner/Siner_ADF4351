# Siner_ADF4351

## Introduction

This is an Arduino library for the Analog Devices ADF4351 wideband synthesizer.
There are other implementations available, but I had a few reasons for implementing it myself.

1. Learn how to develop an Arduino library
1. Familiarize myself with the ADF4351
1. See if I can implement the register calculations using only integer arithmetic
1. See if I can implement simple (e.g. OOK) modulation
1. Have fun

## Wiring

The microcontroller writes ADF4351 registers using a one-way serial protocol.
This library accomodates a two different wirings of the serial pins.
Each wiring corresponds to a different constructor for the Siner_ADF4351 class.

### Wiring 1: Arduino SPI

Arduino Pin | ADF4351 Pin
------------|------------
SCK | CLK
MOSI | DAT
Any digital pin | LE
Any digital pin or 3V3 | CE

This wiring uses the Arduino built-in SPI to communicate with the chip.
In the "simple" example, this wiring is assumed with digital pin 2 connected to CE and digital pin 3 connected to LE.
The constructor for this wiring is `Siner_ADF4351(int pinLoad, SPIClass& spi)`.
The user is responsible for ensuring CE is set high before programming the synth.
The CE pin could also be wired directly to 3V3 to keep the chip enabled.

### Wiring 2: bit banging serial with LE

Arduino Pin | ADF4351 Pin
------------|------------
Any digital pin | CLK
Any digital pin | DAT
Any digital pin | LE
Any digital pin or 3V3 | CE

This wiring does not use the Arduino built-in SPI to communicate with the chip.
Instead it uses two digital pins to create a serial one-way connection using [bit banging](https://en.wikipedia.org/wiki/Bit_banging).
The constructor for this wiring is `Siner_ADF4351(int pinLoad, int pinClock, int pinData)`.
The user is responsible for ensuring CE is set high before programming the synth.
The CE pin could also be wired directly to 3V3 to keep the chip enabled.

## Examples

### simple

The "simple" example programs the chip to a single frequency in setup() and then does nothing in loop().

### serial_sig_gen

This example implements runtime control of the synthesizer frequency, power, and output enable via serial text commands.
