#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Siner_ADF4351.h>

// At a minimum, we need to specify a load-enable (LE) pin,
// but here we also specify the enable (EN) pin.
// If the EN pin is not connected, it needs to be tied high.
// These must match your own wiring.
static const int PIN_ADF4351_EN = 2;
static const int PIN_ADF4351_LE = 3;

// Create a single instance using the pins specified above
// and the SPI object.
Siner_ADF4351 synth = Siner_ADF4351(PIN_ADF4351_EN, PIN_ADF4351_LE, SPI);

static char cmdBuf[32];
static bool synthEnabled = false;

void setup() {
  SPI.begin();
  
  synth.begin();
  
  synth.enable();

  synth.outputEnable = false;
  synth.outputPower = 0;
  synth.frequencyHz = 920000000;

  synth.program();
  
  Serial.begin(9600);
  while (!Serial) {}

  printHelp();
}


void printHelp() {
  Serial.println("HELP: print this help message");
  Serial.println("STAT: print current synth status");
  Serial.println("REGS: print current synth register values");
  Serial.println("ENAB: set the synth enable pin high");
  Serial.println("DISA: set the synth enable pin high");
  Serial.println("FREQ freqHz: set the frequency");
  Serial.println("OUTP (0|1): set the output enable");
  Serial.println("POWE dbPower: set the output power");
}

void printStatus() {
  Serial.print("ENAB:");
  Serial.println((int)synthEnabled);
  Serial.print("FREQ:");
  Serial.println(synth.actualFrequencyHz);
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


void loop() {
  size_t numBytes = Serial.readBytesUntil('\n', cmdBuf, sizeof(cmdBuf) - 1);
  if (numBytes < 4) {
    return;
  }
  cmdBuf[numBytes] = 0;
  if (!strncmp("HELP", cmdBuf, 4)) {
    printHelp();
  } else if (!strncmp("STAT", cmdBuf, 4)) {
    printStatus();
  } else if (!strncmp("REGS", cmdBuf, 4)) {
    printRegs();
  } else if (!strncmp("ENAB", cmdBuf, 4)) {
    synth.enable();
    synthEnabled = true;
  } else if (!strncmp("DISA", cmdBuf, 4)) {
    synth.disable();
    synthEnabled = false;
  } else if (!strncmp("FREQ", cmdBuf, 4)) {
    synth.frequencyHz = strtoul(cmdBuf + 4, NULL, 10);
    synth.program();
  } else if (!strncmp("OUTP", cmdBuf, 4)) {
    synth.outputEnable = strtoul(cmdBuf + 4, NULL, 10);
    synth.program();
  } else if (!strncmp("POWE", cmdBuf, 4)) {
    synth.outputPower = strtol(cmdBuf + 4, NULL, 10);
    synth.program();
  }
}
