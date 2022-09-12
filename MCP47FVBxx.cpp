/* Copyright 2022 Ali Akmal

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   */  
     
     
#include "MCP47FVBxx.h"

MCP47FVBxx::MCP47FVBxx(uint8_t no_of_channels, uint8_t Resolution, uint8_t A1A0_State, float VRefVolt ) { 
  // set ESP log level to Verbose in order to see debugging info
  Serial.setDebugOutput(true);
  // cahnge debug level in tools to see feedback
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Res = Resolution ; // storing resolution in a private variable
  channelCount = no_of_channels;
  VRefVoltage = VRefVolt;
  // passing A1A0 state to drive the device I2C address
  if (A1A0_State == 0x00) { // in case both A1 and A0 are low
    DAC_ADR = DAC_ADR0  ;
  }
  else if (A1A0_State == 0x01) { // in case A1 is low and A0 is high
    DAC_ADR = DAC_ADR1;
  }
  else if (A1A0_State == 0x10) { // in case A1 is high and A0 is low
    DAC_ADR = DAC_ADR2;
  }
  else if (A1A0_State == 0x11) {  // in case both A1 and A0 are high
    DAC_ADR = DAC_ADR3;
  }
  else DAC_ADR = DAC_ADR0; // in case of any erroneous entry
}
// Calculations
// function to mask a bit in the value of the read register if user enters 1
uint16_t MCP47FVBxx::bitMask1(uint16_t regVal, uint8_t ch) { 
  uint8_t check = checkBit(regVal, ch); // first checking if the bit is 0 or 1 
  if (check == 1) {  // if bit is 0 then calculate new value
    // this equation changes only the bit associated with user specificatioin
    regVal |= ( MASK + (RESOLUTION << (ch + VREF_REG)));  
    return regVal;
  }
  else if (check == 2) { // if bit is 1 then return same value
    return  regVal;
  }
  else ESP_LOGD(regVal, "error in reading register value");
  return  regVal; // returns final value to send to device
}
// function to mask a bit in the value of the read register if user enters 0
uint16_t MCP47FVBxx::bitMask0(uint16_t regVal, uint8_t ch) { 

  uint8_t check = checkBit(regVal, ch); // first checking if the bit is 0 or 1 
  if (check == 1) { // if bit is 0 then no change needed.
    return  regVal;
  }
  else if (check == 2) { // if bit is 0 then calculate new value
    // this equation changes only the bit associated with user specification
    regVal &= ( PD_OC - (RESOLUTION << (ch + VREF_REG))); 
    return regVal;
  }
  else ESP_LOGD(regVal, "error in reading register value"); // for error debugging
  return  regVal;
}
// function to check bit value of the read register
uint8_t  MCP47FVBxx::checkBit(uint16_t Cval, uint8_t ch) { 
  Cval &= (BITMASK << ch); // using Bitwise and to check value
  if (Cval != (BITMASK << ch)) {  // comparing values
    return 1; // bit is set to 0
  }
  else if (Cval = (BITMASK << ch)) {
    return 2; // bit is set to 1
  }
  else return 3; // something went wrong
}
 // handels communication to device, private function.
uint8_t  MCP47FVBxx::writeValue(uint16_t val, uint16_t reg) 
{
  Wire.beginTransmission(DAC_ADR); // to write the starting bit and the first control byte
  Wire.write(reg << DAC3 | WRITE ); // Register address along with write command
  Wire.write(val >> VREF_REG); // Write MSB
  Wire.write(val & VREF_BUFFERED); // Write LSB
  return (Wire.endTransmission());
}

// Private function for general call commands
void MCP47FVBxx::generalCall(uint8_t operation) 
{
  Wire.beginTransmission(DAC_ADR); // to write the starting bit and the first control byte
  Wire.write(WRITE); // I2C mandatory byte for the general commands
  Wire.write(operation);// 7bit command (wakeup or reset)
  error = Wire.endTransmission();
  if (error == 0) {
    ESP_LOGD(operation, "General call frame successfully acknowledged");
  }
  else {
    ESP_LOGD((operation, HEX), " General call setting frame acknowledgement failed!");
  }
  delay(100);
}

//Set PowerDown mode 
void MCP47FVBxx::setPowerMode(uint16_t PMode)  // for General call reset and wakeup
{
  if (channelCount == 8) {
    if (PMode == PD_OC) {
      PowerMode = PMode & WIPER_LOCK0;
    }
    else if (PowerMode == PD_PDR1) {
      PowerMode = PMode & WIPER_LOCK1;
    }
    else if (PowerMode == PD_PDR2) {
      PowerMode = PMode & WIPER_LOCK2;
    }
    else PowerMode = PD_NORMAL;
  }
  else PowerMode = PMode;
  error = writeValue(PowerMode, PD_REG);
  if (error == 0) {
    ESP_LOGD((PMode, HEX), "power mode frame successfully acknowledged");
  }
  else {
    ESP_LOGD((PMode, HEX), " Power mode setting frame acknowledgement failed!");
  }
  delay(100);
}
//SETVREF
// This function sets voltage reference mode for a chosen channel and it does't affect other channels
void MCP47FVBxx::setSingleVrefMode(uint16_t VMode, uint8_t ch)  // for General call reset and wakeup
{
  uint16_t regVal;

  regVal = readReg(VREF_REG);
  if (VMode == VREF_BUFFERED) { // if user wants bit to be 0, same command for all devices
    // changes two consequtive bits to change Vref mode
    bitSet(regVal, (2 * ch));
    bitSet(regVal, ((2 * ch) + 1));
  }
  else if (VMode == VREF_NON_BUFFERED) { // wants bit to be 1
    bitClear(regVal, (2 * ch));
    bitSet(regVal, ((2 * ch) + 1));
  }
  else if (VMode == VREF_INTERNAL_BG) { // wants bit to be 1
    bitSet(regVal, (2 * ch));
    bitClear(regVal, ((2 * ch) + 1));
  }
  else if (VMode == VREF_VDD) { // wants bit to be 1
    bitClear(regVal, (2 * ch));
    bitClear(regVal, ((2 * ch) + 1));
  }
  error = writeValue(regVal, VREF_REG); // send to device
  if (error == 0) {
    ESP_LOGD(VREF_REG, "VRef frame successfully acknowledged");
  }
  else {
    ESP_LOGD((VREF_REG, HEX), " VRef setting frame acknowledgement failed!");
  }
  delay(100);

}

void MCP47FVBxx::setVoltRef(uint16_t Vmode)  // for General call reser and wakeup
{
  if (channelCount == 8) {
    if (Vmode == VREF_BUFFERED) {
      val = VREF_BUFFERED | WIPER_LOCK0;
    }
    else if (Vmode == VREF_NON_BUFFERED) {
      val = VREF_NON_BUFFERED | WIPER_LOCK1;
    }
    else if (Vmode == VREF_INTERNAL_BG) {
      val = VREF_INTERNAL_BG | WIPER_LOCK2;
    }
    else if (Vmode == VREF_VDD) {
      val = VREF_VDD;
    }
    else val = VREF_VDD;
  }
  else val = Vmode;

  error = writeValue(val, VREF_REG);
  if (error == 0) {
    ESP_LOGD(VREF_REG, "VRef frame successfully acknowledged");
  }
  else {
    ESP_LOGD((VREF_REG, HEX), " VRef setting frame acknowledgement failed!");
  }
  delay(100);

}
//SETGAIN




void MCP47FVBxx::setGainMode(uint8_t GMode)  // to set gain mode for all channels
{
  uint8_t Gval; // temporary variable to store gain mode
  if (GMode == 1) {
    Gval = GAIN_1X; // gain is the same for both devices in this case.
  }
  // otherwise depends on number of channels
  else if (GMode == 2) {
    if (channelCount == 4) {
      Gval = GAIN_2X;
    }
    else if (channelCount == 8) {
      Gval = GAIN_2X_8CH;
    }
    else Gval = GAIN_2X;
  }
  else Gval = GAIN_1X ;
  error = writeValue(Gval, GAIN_STATUS_REG); // send to device
  if (error == 0) {
    ESP_LOGD(GAIN_STATUS_REG, "Gain setting frame successfully acknowledged");
  }
  else {
    ESP_LOGD((GAIN_STATUS_REG, HEX), " Gain setting frame acknowledgement failed!");
  }
  delay(100);
}

void MCP47FVBxx::setSingleChannelGain(uint16_t GMode, uint8_t ch)  // for setting gain in a single channel
{
  uint16_t regVal, tempV;

  regVal = readReg(GAIN_STATUS_REG);
  // using same Bit masking as in voltage reference single mode set.
  if (GMode == 1) { // wants bit to be 0
    tempV = bitMask0(regVal, ch);
  }
  else if (GMode == 2) { // wants bit to be 1
    tempV = bitMask1(regVal, ch);
  }
  error = writeValue(tempV, GAIN_STATUS_REG);
  if (error == 0) {
    ESP_LOGD(GAIN_STATUS_REG, "Gain setting frame successfully acknowledged");
  }
  else {
    ESP_LOGD((GAIN_STATUS_REG, HEX), " Gain setting frame acknowledgement failed!");
  }
  delay(100);
}
void MCP47FVBxx::DACwakeup()  // for General call command wakeup
{
  generalCall(WAKE_UP);

}
void MCP47FVBxx::DACreset()  // for General call command reset
{
  generalCall(RESET);
}


//SetVout
// Function overloaded to set output voltage for single , double, quad and octal channels
void MCP47FVBxx::setVout(float Val, uint8_t regAddr)
{
  // calculte actual value to be sent to the device
  tempValvout = (Val * ( RESOLUTION << Res )) / VRefVoltage ; 
  error = writeValue(tempValvout, regAddr);
  if (error == 0) {
    ESP_LOGD(regAddr, "frame successfully acknowledged");
  }
  else {
    ESP_LOGD((regAddr, HEX), " frame acknowledgement failed!");
  }


}
void MCP47FVBxx::setVout(float Val, uint8_t regAddr0, uint8_t regAddr1)
{
  setVout(Val, regAddr0);
  delay(10);
  setVout(Val, regAddr1);

}
void MCP47FVBxx::setVout(float Val, uint8_t regAddr0, uint8_t regAddr1, uint8_t regAddr2, uint8_t regAddr3)
{
  setVout(Val, regAddr0, regAddr1);
  delay(10);
  setVout(Val, regAddr2, regAddr3);

}

void MCP47FVBxx::setVout(float Val)
{
  if (channelCount == 4) {
    setVout(Val, DAC0, DAC1, DAC2, DAC3);
  }
  else if (channelCount == 8) {
    setVout(Val, DAC0, DAC1, DAC2, DAC3);
    delay(10);
    setVout(Val, DAC4, DAC5, DAC6, DAC7);
  }
  else {
    ESP_LOGD(channelCount, "number of channels is not supported");
  }
}

//READ REG
uint16_t MCP47FVBxx::readReg(uint8_t regAdr)  // to read a regiter value
{
  uint8_t tempVal;
  uint16_t tempReg;
  Wire.beginTransmission(DAC_ADR); // to write the starting bit and the first control byte
  Wire.write(regAdr << DAC3 | READ ); // Register address along with write command
  Wire.endTransmission();
  Wire.requestFrom(DAC_ADR, (uint8_t)2); //Read in reg value
  tempReg = Wire.read();//Read in MSB
  tempReg <<= 8;
  tempVal = Wire.read();  //Read in LSB
  tempReg = tempReg | tempVal;
  Wire.endTransmission();

  return tempReg;
}
