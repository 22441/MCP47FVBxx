#ifndef MCP47FVBxx_H
#define MCP47FVBxx_H
#include <Wire.h>
#include "esp32-hal-log.h"
#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Device register adresses
#define DAC_ADR0 0x60 // Pins A1 and A0 are LOW.
#define DAC_ADR1 0x61 // Pin A0 is HIGH, Pin A1 is LOW.
#define DAC_ADR2 0x62 // Pin A0 LOW, Pin A1 is HIGH.
#define DAC_ADR3 0x63 // Pins A1 and A0 are HIGH.
#define DAC0 0x00 // DAC0 register address
#define DAC1 0x01 // DAC1 register address
#define DAC2 0x02 // DAC2 register address
#define DAC3 0x03 // DAC3 register address
#define DAC4 0x04 // DAC4 register address
#define DAC5 0x05 // DAC5 register address
#define DAC6 0x06 // DAC6 register address
#define DAC7 0x07 // DAC7 register address
#define VREF_REG 0x08 // Voltage reference register address
#define PD_REG 0x09 // Power down register address
#define GAIN_STATUS_REG 0x0A // Gain and status register address
#define WIPERLOCK_REG 0x0B // Wiper lock technology register address


// Device operation modes

// Voltage reference 4 channels
#define VREF_BUFFERED 0xFF // Buffer enabled
#define VREF_NON_BUFFERED 0xAA // Non buffered
#define VREF_INTERNAL_BG 0x55  //1.22V typical
#define VREF_VDD 0x00 // VDD is used as a refrence voltage


// Power Down 4 ch
#define PD_OC 0xFF // Vout is open circuit
#define PD_PDR1 0xAA // Vout is connected to 125K ohm pulldown resistor
#define PD_PDR2 0x55 // Vout is connected to 1k ohm pulldown resistor
#define PD_NORMAL 0x0000 // Normal operation mode


// DAC wiperlock status register
#define WIPER_LOCK0 0xFFFF // DAC Wiper and configuration are locked
#define WIPER_LOCK1 0xAAAA // DAC Wiper and configuration are locked
#define WIPER_LOCK2 0x5555 // Not implemented
#define WIPER_UNLOCK 0x0000 // DAC Wiper and configuration are unlocked

// Gain control and system status register
#define GAIN_1X 0x0000 // Vout is not changed
#define GAIN_2X 0x0F00 // Vout is multipled by 2
#define GAIN_2X_8CH 0xFF00 // gain for 8 channel DAC


// Genaral call commands
#define WAKE_UP 0x0A   // a special instructoin to wake up the dac.
#define RESET 0x06  // a special instruction to reset the dac to factory defaults.

// main commmands
#define WRITE 0x00 // I2C write command to append the byte
#define READ 0x06 //  I2C Read command to append the byte
#define RESOLUTION 0x01 // private contstant to drive different resolutions
#define BITMASK 0x100  // private conttant used in Bitmask functions
#define MASK 0x4000  // // private conttant used in Bitmask functions





class MCP47FVBxx {
  public:
    // Constructor
    MCP47FVBxx(uint8_t no_of_channels = 8, uint8_t Resolution = 12, uint8_t A1A0_State = 00, float VRefVolt = 1);

    // Methods
    void setGainMode(uint8_t GMode);
    void setSingleChannelGain(uint16_t GMode, uint8_t ch);
    void setVoltRef(uint16_t Vmode);
    void setPowerMode(uint16_t PMode);
    void setVout(float Val, uint8_t regAddr);
    void setVout(float Val, uint8_t regAddr0, uint8_t regAddr1);
    void setVout(float Val);
    void setSingleVrefMode(uint16_t VMode, uint8_t ch);
    void DACwakeup();
    void DACreset();
    uint16_t readReg(uint8_t regAdr);
    float VRefVoltage;




  private:
    void generalCall(uint8_t operation);
    uint16_t bitMask1(uint16_t regVal, uint8_t ch);
    uint16_t bitMask0(uint16_t regVal, uint8_t ch);
    uint8_t checkBit(uint16_t Cval, uint8_t ch);
    uint8_t writeValue(uint16_t val, uint16_t reg);
    void setVout(float Val, uint8_t regAddr0, uint8_t regAddr1, uint8_t regAddr2, uint8_t regAddr3);
    uint8_t error;
    uint16_t val;
    uint8_t channelCount;
    uint16_t tempValvout;
    uint16_t Res;
    uint16_t Rval;
    uint16_t PowerMode;
    uint8_t DAC_ADR;

};











#endif
