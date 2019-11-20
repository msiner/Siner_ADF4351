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

This library accomodates a few different methods of wiring the ADF4351 to a microcontroller.
Each wiring corresponds to a different constructor for the Siner_ADF4351 class.

### Wiring 1: SPI with LE and EN

Arduino Pin | ADF4351 Pin
------------|------------
SCK | CLK
MOSI | DAT
Any digital pin | EN
Any digital pin | LE

This wiring uses the Arduino built-in SPI to communicate with the chip.
In the "simple" example, this wiring is assumed with digital pin 2 connected to EN and digital pin 3 connected to LE.
The constructor for this wiring is `Siner_ADF4351(int pinEnable, int pinLoad, SPIClass& spi)`.

### Wiring 2: SPI with LE

Arduino Pin | ADF4351 Pin
------------|------------
SCK | CLK
MOSI | DAT
3V3 | EN
Any digital pin | LE

This wiring uses the Arduino built-in SPI to communicate with the chip.
The EN pin is tied high to keep the chip enabled.
The constructor for this wiring is `Siner_ADF4351(int pinLoad, SPIClass& spi)`.

### Wiring 3: bit banging serial with LE

Arduino Pin | ADF4351 Pin
------------|------------
Any digital pin | CLK
Any digital pin | DAT
3V3 | EN
Any digital pin | LE

This wiring does not use the Arduino built-in SPI to communicate with the chip.
Instead it uses two digital pins to create a serial one-way connection using [bit banging](https://en.wikipedia.org/wiki/Bit_banging).
The EN pin is tied high to keep the chip enabled.
The constructor for this wiring is `Siner_ADF4351(int pinLoad, int pinClock, int pinData)`.

### Wiring 3: bit banging serial with LE

Arduino Pin | ADF4351 Pin
------------|------------
Any digital pin | CLK
Any digital pin | DAT
3V3 | EN
Any digital pin | LE

This wiring does not use the Arduino built-in SPI to communicate with the chip.
Instead it uses two digital pins to create a serial one-way connection using [bit banging](https://en.wikipedia.org/wiki/Bit_banging).
The constructor for this wiring is `Siner_ADF4351(int pinEnable, int pinLoad, int pinClock, int pinData)`.

## Examples

### simple

The "simple" example programs the chip to a single frequency in setup() and then does nothing in loop().
