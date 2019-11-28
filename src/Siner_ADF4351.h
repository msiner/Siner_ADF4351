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


class Siner_ADF4351 {

public:
  uint32_t frequencyHz = 0;
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
  uint32_t registers[6] = {0xFFFFFFFF};

  Siner_ADF4351(int pinLoad, SPIClass& spi);
  Siner_ADF4351(int pinEnable, int pinLoad, SPIClass& spi);
  Siner_ADF4351(int pinLoad, int pinClock, int pinData);
  Siner_ADF4351(int pinEnable, int pinLoad, int pinClock, int pinData);
  ~Siner_ADF4351(void);

  void begin(void);
  void enable(void);
  void disable(void);
  bool computeRegisterValues(void);
  void writeRegister(uint32_t regVal);
  void writeRegisters(void);
  bool program(void);

protected:
  int pinEnable = -1;
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