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


#include <SPI.h>
#include <Siner_ADF4351.h>


// On setup, the synth will be programmed with this frequency.
static const uint32_t FREQUENCY_HZ = 920000000;

// At a minimum, we need to specify a load-enable (LE) pin,
// but here we also specify the enable (EN) pin.
// If the EN pin is not connected, it needs to be tied high.
// These must match your own wiring.
static const int PIN_ADF4351_EN = 2;
static const int PIN_ADF4351_LE = 3;

// Create a single instance using the pins specified above
// and the SPI object.
Siner_ADF4351 synth = Siner_ADF4351(PIN_ADF4351_EN, PIN_ADF4351_LE, SPI);


void setup() {
  // The synth uses SPI, so call its begin() first
  SPI.begin();

  // Initialize the synth instance
  synth.begin();

  // This will raise the EN pin to enable the chip
  synth.enable();

  // Many of the cheap AD4351 boards use a 25 MHz crystal, but
  // this needs to change if you are using an external 10 MHz
  // or your board has a different crystal.
  synth.referenceHz = 25000000;

  // Configure the output frequency
  synth.frequencyHz = FREQUENCY_HZ;

  // Enable the primary RF output
  synth.outputEnable = true;

  // program() will determine the correct register values and
  // then write them to the chip.
  synth.program();
}


void loop() {
  // Do nothing
}
