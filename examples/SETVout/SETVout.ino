#include "MCP47FVBxx.h"

uint8_t Channels = 8;
uint8_t Resolution = 12;
uint8_t A1A0_state = 0x00; //options are 0x01 if A1 is low and A0 is high, 0x10, 0x11.
float VRef = 1.65;
MCP47FVBxx DAC(Channels, Resolution, A1A0_state, VRef);

void setup() {
  Wire.begin();
  Serial.begin(115200);
  DAC.setGainMode(1);// 1 for normal gain mode, 2 for 2x gain mode
  DAC.setVoltRef(VREF_BUFFERED);//  other options are VREF_NON_BUFFERED, VREF_INTERNAL_BG, VREF_VDD
  delay(100);
}

void loop() {
  DAC.setVout(1.0, DAC0, DAC1); // depending on number of channels addresses from DAC0 to DAC7
  //DAC.setVout(1.0); // sets output voltage for all channels
  delay(100);
}
