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


#include <errno.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Siner_ADF4351.h>


static const int PIN_ADF4351_CE = 2; // chip enable (CE)
static const int PIN_ADF4351_LE = 3; // load enable (LE)

// Create a synth instance that uses default SPI instance for
// serial communication with the chip
//Siner_ADF4351 synth = Siner_ADF4351(PIN_ADF4351_LE, SPI);

// Create a synth instance that uses bit-banging SPI
static const int PIN_ADF4351_DATA = 4; // bit-banging SPI data (DATA)
static const int PIN_ADF4351_CLK = 5; // bit-baning SPI clock (CLK)
Siner_ADF4351 synth = Siner_ADF4351(PIN_ADF4351_LE, PIN_ADF4351_CLK, PIN_ADF4351_DATA);

static char cmdBuf[32];
static char* startPtr = NULL;
static char* endPtr = NULL;


void setup() {
  // The synth uses SPI, so call its begin() first
  //SPI.begin();

  // Initialize the synth instance
  synth.begin();

  // This will raise the EN pin to enable the chip
  pinMode(PIN_ADF4351_CE, OUTPUT);
  digitalWrite(PIN_ADF4351_CE, HIGH);

  // Many of the cheap ADF4351 boards use a 25 MHz crystal, but
  // this needs to change if you are using an external 10 MHz
  // or your board has a different crystal.
  synth.referenceHz = 25000000;

  // Disable the RF output to start
  synth.outputEnable = false;

  // Configure the output frequency
  synth.frequencyHz = 920000000;

  // Enable the primary RF output
  synth.outputEnable = true;

  // program() will determine the correct register values and
  // then write them to the chip.
  synth.program();
  
  // Wait for user to connect via serial (Serial Monitor in Arduino IDE)
  Serial.begin(9600);
  while (!Serial) {}

  printHelp();
}


void printHelp() {
  Serial.println("HELP: print this help message");
  Serial.println("STAT: print synth status");
  Serial.println("REGS: print synth register values");
  Serial.println("ENAB (0|1): set the synth enable pin high");
  Serial.println("FREQ freqHz: set the frequency");
  Serial.println("OUTP (0|1): set the output enable");
  Serial.println("POWE dbPower: set the output power");
  Serial.println("MATH: print the intermediate results");
}

void printStatus() {
  Serial.print("ENAB:");
  Serial.println(digitalRead(PIN_ADF4351_CE));
  Serial.print("FREQ:");
  Serial.println(synth.frequencyHz);
  Serial.print("OUTP:");
  Serial.println(synth.outputEnable);
  Serial.print("POWE:");
  Serial.println(synth.outputPower);
}


void printRegs() {
  for (int8_t regId = 0; regId < 6; regId++) {
    Serial.print("REG");
    Serial.print(regId);
    Serial.print(":");
    Serial.println(synth.registers[regId], HEX);
  }
}


void printMath() {
  if (synth.resultPrescaler) {
    Serial.println("PRE:8/9");
  } else {
    Serial.println("PRE:4/5");
  }
  Serial.print("INT:");
  Serial.println(synth.resultInt);
  Serial.print("FRAC:");
  Serial.println(synth.resultFrac);
  Serial.print("MOD:");
  Serial.println(synth.resultMod);
  Serial.print("DIV:");
  Serial.println(synth.resultDiv);
}


void loop() {
  size_t numBytes = Serial.readBytesUntil('\n', cmdBuf, sizeof(cmdBuf) - 1);
  if (numBytes == 0) {
    return;
  } else if (numBytes < 4) {
    Serial.println("ERROR: invalid input");
    return;
  }
  
  // Null-terminate the buffer
  cmdBuf[numBytes] = 0;

  if (!strncmp("HELP", cmdBuf, 4)) {
    printHelp();
  } else if (!strncmp("STAT", cmdBuf, 4)) {
    printStatus();
  } else if (!strncmp("REGS", cmdBuf, 4)) {
    printRegs();
  } else if (!strncmp("MATH", cmdBuf, 4)) {
    printMath();
  } else if (!strncmp("ENAB", cmdBuf, 4)) {
    errno = 0;
    startPtr = cmdBuf + 4;
    bool tmp = strtoul(startPtr, &endPtr, 10);
    if (errno == 0 && endPtr != startPtr) {
      if (tmp) {
        digitalWrite(PIN_ADF4351_CE, HIGH);
      } else {
        digitalWrite(PIN_ADF4351_CE, LOW);
      }
      Serial.print("ENAB:");
      Serial.println(tmp);
    } else {
      Serial.println("ERROR: invalid argument");
    }
  } else if (!strncmp("FREQ", cmdBuf, 4)) {
    errno = 0;
    startPtr = cmdBuf + 4;
    uint32_t tmp = strtoul(startPtr, &endPtr, 10);
    if (errno == 0 && endPtr != startPtr) {
      synth.frequencyHz = tmp;
      synth.program();
      Serial.print("FREQ:");
      Serial.println(synth.frequencyHz);
    } else {
      Serial.println("ERROR: invalid argument");
    }
  } else if (!strncmp("OUTP", cmdBuf, 4)) {
    errno = 0;
    startPtr = cmdBuf + 4;
    bool tmp = strtoul(startPtr, &endPtr, 10);
    if (errno == 0 && endPtr != startPtr) {
      synth.outputEnable = tmp;
      synth.program();
      Serial.print("OUTP:");
      Serial.println(synth.outputEnable);
    } else {
      Serial.println("ERROR: invalid argument");
    }
  } else if (!strncmp("POWE", cmdBuf, 4)) {
    errno = 0;
    startPtr = cmdBuf + 4;
    int8_t tmp = strtol(startPtr, &endPtr, 10);
    if (errno == 0 && endPtr != startPtr) {
      synth.outputPower = tmp;
      synth.program();
      Serial.print("POWE:");
      Serial.println(synth.outputPower);
    } else {
      Serial.println("ERROR: invalid argument");
    }
  } else {
    Serial.println("ERROR: unrecognized command");
  }
}
