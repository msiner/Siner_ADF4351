/*
Copyright (c) 2019 Mark Siner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "Siner_ADF4351.h"
#include "binary_gcd.h"


Siner_ADF4351::Siner_ADF4351(int pinLoad, SPIClass& spi) :
  pinLoad(pinLoad), spi(&spi) 
{

}

Siner_ADF4351::Siner_ADF4351(int pinEnable, int pinLoad, SPIClass& spi) :
  pinEnable(pinEnable), pinLoad(pinLoad), spi(&spi) 
{

}

Siner_ADF4351::Siner_ADF4351(int pinLoad, int pinClock, int pinData) :
  pinEnable(pinEnable), pinLoad(pinLoad),  pinClock(pinClock), pinData(pinData) 
{

}
Siner_ADF4351::Siner_ADF4351(int pinEnable, int pinLoad, int pinClock, int pinData) :
  pinEnable(pinEnable), pinLoad(pinLoad),  pinClock(pinClock), pinData(pinData) 
{

}

Siner_ADF4351::~Siner_ADF4351() {

}

void Siner_ADF4351::begin() {
  // Configure the direction of any specified pins
  pinMode(pinLoad, OUTPUT);
  if (pinEnable >= 0) {
    pinMode(pinEnable, OUTPUT);
  }
  if (pinClock >= 0) {
    pinMode(pinClock, OUTPUT);
    pinMode(pinData, OUTPUT);
  }
}

void Siner_ADF4351::enable() {
  if (pinEnable >= 0) {
    digitalWrite(pinEnable, HIGH);
  }
}

void Siner_ADF4351::disable() {
  if (pinEnable >= 0) {
    digitalWrite(pinEnable, LOW);
  }
}

bool Siner_ADF4351::computeRegisterValues() {
  // Table of valid values for DIV
  static const uint8_t divBankSize = 7;
  static const uint32_t divBank[divBankSize] = {1, 2, 4, 8, 16, 32, 64};
  uint8_t newRegMask = 0;
  uint32_t newRegisters[6] = {
    0x00000000,
    0x00000001,
    0x00004e42,
    0x000004b3,
    0x00ac8024,
    0x00580005
  };


  // Bin the requested frequency to the nearest frequency in
  // our predetermined resolution.
  // The ADF4351 might be capable of finer resolutions, but the parameter
  // calculations done here are simple and non-iterative.
  uint32_t tenKHzUnits = frequencyHz / 10000;
  uint32_t tenKHzRem = frequencyHz % 10000;
  frequencyHz = 10000 * (tenKHzUnits + (tenKHzRem >= 5000));

  // Determine prescaler
  if (frequencyHz > 3600000000) {
    // prescaler is 8/9
    newRegisters[1] |= 1 << 27;
  } else {
    // prescaler is 4/5
    newRegisters[1] &= ~(1 << 27);
  }

  // Calculate the PFD frequency
  uint32_t pfdHz = referenceHz;
  if (referenceDouble) {
    pfdHz *= 2;
  }
  if (referenceDivide) {
    pfdHz /= 2;
  }

  // Get the freq to pfd ratio in whole steps
  uint32_t ratio = frequencyHz / pfdHz;

  // Determine the correct DIV value.
  // We do this first because DIV is the most constrained parameter.
  uint32_t rfDiv = 0;
  // The internal VOSC has a minimum freq of 2.2 GHz.
  // So we need to find the smallest DIV that can get up to 2.2 GHz.
  // Remember, 'ratio' is in whole steps.
  // So ratio * pfdHz does not necessarily equal frequencyHz.
  uint32_t minDiv = 2200000000 / (ratio * pfdHz);
  // Walk through the bank of valid DIV values until we find the
  // smallest one that is greater than the minimum.
  for (int8_t divId = 0; divId < divBankSize; divId++) {
    if (divBank[divId] > minDiv) {
      // The DIV value in the register matches the bank index.
      // That is, we want the index, not the actual value.
      rfDiv = divId;
      break;
    }
  }

  // Now that we have solved for DIV, we solve for INT, FRAC, and MOD.
  // Get the actual DIV value being used from the bank.
  uint32_t rfDivVal = divBank[rfDiv];
  // INT is the easiest to solve for now that we have DIV.
  // Just solve for the whole steps.
  uint32_t nInt = (rfDivVal * frequencyHz) / pfdHz;
  // To solve FRAC and MOD, we need to use the remainder that INT leaves.
  uint32_t nRem = (rfDivVal * frequencyHz) % pfdHz;
  // Reduce the fraction nRem/pfdHz by solving for the GCD.
  // Reducing the fraction gets FRAC and MOD values in the correct range.
  uint32_t gcdVal = binary_gcd(nRem, pfdHz);
  uint32_t frac = nRem / gcdVal;
  uint32_t mod = pfdHz / gcdVal;

  // Determine the correct phase value.
  // The phase value must be <= MOD, but it can still go from 0 to 360 deg.
  // That means the resolution is limited to 360 / MOD
  uint32_t phaseResolution = 360 / mod;
  uint32_t phaseValue = phaseDegrees / phaseResolution;
  
  // Determine correct output power register value
  uint32_t outputPowerVal = 0;
  if (outputPower >= 5) {
    outputPowerVal = 3;
    outputPower = 5;
  } else if (outputPower >= 2) {
    outputPowerVal = 2;
    outputPower = 2;
  } else if (outputPower >= -1) {
    outputPowerVal = 1;
    outputPower = -1;
  } else {
    outputPower = -4;  
  }
  
  // Determine correct aux power register value
  uint32_t auxPowerVal = 0;
  if (auxPower >= 5) {
    auxPowerVal = 3;
    auxPower = 5;
  } else if (auxPower >= 2) {
    auxPowerVal = 2;
    auxPower = 2;
  } else if (auxPower >= -1) {
    auxPowerVal = 1;
    auxPower = -1;
  } else {
    auxPower = -4;
  }

  // Register 0
  newRegisters[0] |= (nInt & 0xffff) << 15;
  newRegisters[0] |= (frac & 0xfff) << 3;

  // Register 1
  newRegisters[1] |= (mod & 0xfff) << 3;
  newRegisters[1] |= (phaseValue & 0xfff) << 15;
  newRegisters[1] |= (phaseAdjust & 0x1) << 28; 

  // Register 2
  newRegisters[2] |= (referenceDivide & 0x1) << 24;
  newRegisters[2] |= (referenceDouble & 0x1) << 25;

  // Register 4
  newRegisters[4] &= ~(0x3 << 3);
  newRegisters[4] |= (outputPowerVal & 0x3) << 3;

  newRegisters[4] &= ~(0x1 << 5);
  newRegisters[4] |= (outputEnable & 0x1) << 5;

  newRegisters[4] &= ~(0x3 << 6);
  newRegisters[4] |= (auxPowerVal & 0x3) << 6;

  newRegisters[4] &= ~(0x1 << 8);
  newRegisters[4] |= (auxEnable & 0x1) << 8;

  newRegisters[4] &= ~(0x1 << 9);
  newRegisters[4] |= ((!auxDivide) & 0x1) << 9;

  newRegisters[4] &= ~(0x1 << 10);
  newRegisters[4] |= (muteTillLockDetect & 0x1) << 10;

  newRegisters[4] &= ~(0x7 << 20);
  newRegisters[4] |= (rfDiv & 0x7) << 20;

  // See which registers actually need to be updated
  for (uint8_t regId=0; regId < 6; regId++) {
    if (registers[regId] != newRegisters[regId]) {
      newRegMask |= 1 << regId;
    }
    registers[regId] = newRegisters[regId];
  }

  // Check the values that are double-buffered to see if
  // register 0 needs to be written
  if ((mod != lastMod) ||
      (phaseValue != lastPhaseValue) ||
      (referenceDouble != lastReferenceDouble) ||
      (referenceDivide != lastReferenceDivide)){
    newRegMask |= 1;
  }
  lastMod = mod;
  lastPhaseValue = phaseValue;
  lastReferenceDouble = referenceDouble;
  lastReferenceDivide = referenceDivide;
  
  registerMask = newRegMask;
  return true;
}

void Siner_ADF4351::writeRegister(uint32_t regVal) {
  // Cache the register value if it has a valid control field
  uint32_t regIndex = regVal & 0x7;
  if (regIndex < 6) {
    registers[regIndex] = regVal;
  }

  if (spi) {
    // Use SPI object passed in constructor
    spi->beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

    // Drop the LoadEnable pin before writing a new register
    digitalWrite(pinLoad, LOW);
    delayMicroseconds(1);

    // Write the register MSB first byte-by-byte
    spi->transfer((regVal >> 24) & 0xff);
    spi->transfer((regVal >> 16) & 0xff);
    spi->transfer((regVal >> 8) & 0xff);
    spi->transfer(regVal & 0xff);

    // Pulse the LoadEnable pin to load the register
    digitalWrite(pinLoad, HIGH);
    delayMicroseconds(1);
    digitalWrite(pinLoad, LOW);
    spi->endTransaction();
  } else {
    // Use bit-banging
    // Drop the LoadEnable pin before writing a new register
    digitalWrite(pinLoad, LOW);
    delayMicroseconds(1);

    // Walk through each bit, MSB first
    for (int i = 31; i >= 0; i--) {
      // Drop CLK
      digitalWrite(pinClock, LOW);
      delayMicroseconds(1);

      // Set DATA
      if ((regVal >> i) & 1) {
        digitalWrite(pinData, HIGH);
      } else {
        digitalWrite(pinData, LOW);
      }
      delayMicroseconds(1);

      // Raise CLK
      digitalWrite(pinClock, HIGH);
      delayMicroseconds(1);
    }

    // Drop CLK one final time
    digitalWrite(pinClock, LOW);
    delayMicroseconds(1);

    // Pulse the LoadEnable pin to load the register
    digitalWrite(pinLoad, HIGH);
    delayMicroseconds(1);
    digitalWrite(pinLoad, LOW);
  }
}

void Siner_ADF4351::writeRegisters() {
  // Check and write, if necessary, in reverse order
  // to ensure reg 0 is written last to latch in
  // double-buffered values
  for (int8_t regId = 5; regId >= 0; regId--) {
    if ((registerMask >> regId) & 1) {
      writeRegister(registers[regId]);
    }
  }
  registerMask = 0;
}

bool Siner_ADF4351::program() {
  if (computeRegisterValues()) {
    writeRegisters();
    return true;
  }
  return true;
}