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


#ifndef SINER_ADF4351_H
#define SINER_ADF4351_H

#include <Arduino.h>
#include <SPI.h>


const uint8_t ADF4351_NUM_REGS = 6;

/**
 * Siner_ADF4351 implements dynamic runtime control of an ADF4351
 * RF synthesizer.
 * 
 * The intended method of operation is to set the public attributes, excluding
 * the ones with a "result" prefix, to the desired values, and then call
 * program().
 **/
class Siner_ADF4351 {

public:
  // desired output frequency in Hz
  uint32_t frequencyHz = 0;
  // reference input frequency in Hz
  uint32_t referenceHz = 25000000;
  bool outputEnable = true;
  int8_t outputPower = -4;
  bool auxEnable = false;
  int8_t auxPower = -4;
  bool auxDivide = true;
  bool muteTillLockDetect = true;
  bool phaseAdjust = false;
  uint16_t phaseDegrees = 1;
  bool referenceDouble = false;
  bool referenceDivide = false;
  bool integerN = false;
  uint32_t registers[ADF4351_NUM_REGS] = {0xFFFFFFFF};
  
  uint32_t resultInt = 0;
  uint32_t resultDiv = 0;
  uint32_t resultFrac = 0;
  uint32_t resultMod = 0;
  uint32_t resultPrescaler = 0;
  uint32_t resultFrequency = 0;
  uint32_t resultPfdHz = 0;

  /**
   * The empty constructor enables use of computeRegisterValues()
   * without configuring communication to a chip.
   * In this configuration, writeRegister() is a NOOP.
   **/
  Siner_ADF4351(void);
  // Use built-in SPI peripheral
  Siner_ADF4351(int pinLoad, SPIClass& spi);
  // Use bit-banged SPI via GPIO pins
  Siner_ADF4351(int pinLoad, int pinClock, int pinData);
  ~Siner_ADF4351(void);

  void begin(void);
  bool computeRegisterValues(void);
  void writeRegister(uint32_t regVal);
  void writeRegisters(void);
  bool program(void);

protected:
  int pinLoad = -1;
  int pinClock = -1;
  int pinData = -1;
  SPIClass* spi = NULL;
  uint8_t registerMask = 0;
  uint32_t lastMod = 0;
  uint32_t lastPhaseValue = 0;
  bool lastReferenceDouble = false;
  bool lastReferenceDivide = false;
};


#endif // SINER_ADF4351_H