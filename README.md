# MCP47FVBxx
this library can be used to control the output voltage of the MCP47FVBxx family of digital to analog converters,
as well as managing communication with the device, made for arduino.

created by Ali Akmal

#Installation
--------------------------------------------------------------------------------

To install this library, just place this entire folder as a subfolder in your
Arduino/lib/targets/libraries folder.

#Building
--------------------------------------------------------------------------------

After this library is installed,to use it in a sketch, go to the Sketch | Import Library menu and
select MCP47FVBXXThis will add a corresponding line to the top of your sketch:
#include <MCP47FVBxx.h>

To stop using this library, delete that line from your sketch.

#Using the library
--------------------------------------------------------------------------------

- First create an object from MCP47FVBxx and pass the following variables: Number of channels, Resolution, A1 pin and A0 pin state (0 for low and 1 for high), Voltage reference
 value (connected to pins Vref0 and Vref1).
- Set Voltage reference mode.
-Set Gain (optional).
-Set power mode (optional).
- Set output voltage.

List of methods and their functionality
--------------------------------------------------------------------------------
- setGainMode(uint8_t GainMode); //1 for normal mode, 2 for output multiplied by 2.

- setSingleChannelGain(uint16_t GainMode, uint8_t channel); // sets gain for a single channel without affecting other channels.

- setVoltRef(uint16_t Vmode);  // options VREF_BUFFERED , VREF_NON_BUFFERED, VREF_INTERNAL_BG, VREF_VDD .

- setPowerMode(uint16_t PowerMode); // option modes : PD_OC , PD_PDR1, PD_PDR2, PD_NORMAL.

- setVout(float Value, uint8_t registerAddress);// sets output voltage for a single channel.

- setVout(float Val, uint8_t regAddr0, uint8_t regAddr1); // sets output voltage for a 2 channels.

- setVout(float Val); // sets output voltage for all channels.

- setSingleVrefMode(uint16_t VMode, uint8_t channel); sets vlotage reference mode for a single channel without affecting other channels.

- DACwakeup();

- DACreset();

- readReg(uint8_t regAdr); returns actual 16 bit value of the register.


-- for more information about specific operating modes please check the device datasheet.
   

