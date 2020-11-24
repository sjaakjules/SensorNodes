/*
* Project SensorNode
* Description: Main script
* Author:  Julian Rutten - jrutten@swin.edu.au
* Date:    11/07/2018
*/

//=======================================
//  dependency guards
#if defined(PARTICLE)
#include <Particle.h>
#if(SYSTEM_VERSION < 0x00060100)
#error requires system target 0.6.1 or above
#endif
#endif

#if (ARDUINO >= 100) || defined(PARTICLE)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#if !defined(PARTICLE)
#include "Wire.h"
#endif

//======================================
//  Dependencies

#include "Sensors/TSL2561.h"
#include "SensorBase.h"
#include "SensorController.h"
#include <vector>
#include <string>



//using namespace std;

TSL2561::TSL2561() : SensorBase(nReadings)
{
  //  SensorBase(nReadings);
  _name = "TSL2561 Lux/visible/IR Light Sensor";
  //  _nSensorReadings = 3;
}

bool TSL2561::Connect()
{
  if (!_hasSetup) {
    if (!tsl.begin()) {
      Serial.println("Could not find a valid TSL2561 sensor, check wiring!");
      _hasSetup = false;
    }else{
      Serial.println("TSL2561 Connected!");
      _hasSetup = true;
      tsl.setGain(TSL2591_GAIN_LOW);
      tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
      UpdateReading();
    }
  }
  return _hasSetup;
}

bool TSL2561::UpdateReading()
{
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;

  latestreading[3]=full;
  latestreading[1]=full - ir;
  latestreading[2]=ir;
  latestreading[0]=tsl.calculateLux(full, ir);

  return true;
}


void TSL2561::PrintToSerial()
{
  Serial.println(_name);

  Serial.print("Full Light Spectrum = ");
  Serial.print(latestreading[0]);
  Serial.println(" Lux");

  Serial.print("Visible Light = ");
  Serial.print(latestreading[1]);
  Serial.println(" Unitless");

  Serial.print("IR Light = ");
  Serial.print(latestreading[2]);
  Serial.println(" Unitless");

  Serial.print("Lux Levels = ");
  Serial.print(latestreading[3]);
  Serial.println(" Unitless");
  Serial.println();


}



/*
===================================
notes and info

// I2C address options
#define TSL2591_ADDR_LOW          (0x29)


// You can change the gain on the fly, to adapt to brighter/dimmer light situations
//tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
//tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

// Changing the integration time gives you a longer time over which to sense light
// longer timelines are slower, but are good in very low light situtations!
//tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
// tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
// tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
// tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
// tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

functions


boolean   begin   ( void );
void      enable  ( void );
void      disable ( void );

float     calculateLux  ( uint16_t ch0, uint16_t ch1 );
void      setGain       ( tsl2591Gain_t gain );
void      setTiming     ( tsl2591IntegrationTime_t integration );
uint16_t  getLuminosity (uint8_t channel );
uint32_t  getFullLuminosity ( );

/**************************************************************************/
/*
    Shows how to perform a basic read on visible, full spectrum or
    infrared light (returns raw 16-bit ADC values)
*/
/**************************************************************************/
/*
void simpleRead(void)
{
  // Simple data read example. Just read the infrared, fullspecrtrum diode
  // or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);

  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("Luminosity: "));
  Serial.println(x, DEC);
}

/**************************************************************************/
/*
    Show how to read IR and Full Spectrum at once and convert to lux
*/
/**************************************************************************/
/*
void advancedRead(void)
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  Serial.print(F("Lux: ")); Serial.println(tsl.calculateLux(full, ir), 6);
}

/**************************************************************************/
/*
    Performs a read using the Adafruit Unified Sensor API.
*/
/**************************************************************************/
/*
void unifiedSensorAPIRead(void)
{
  /* Get a new sensor event */
  /*
  sensors_event_t event;
  tsl.getEvent(&event);

  /* Display the results (light is measured in lux) */
  /*
  Serial.print(F("[ ")); Serial.print(event.timestamp); Serial.print(F(" ms ] "));
  if ((event.light == 0) |
      (event.light > 4294966000.0) |
      (event.light <-4294966000.0))
  {
    /* If event.light = 0 lux the sensor is probably saturated */
    /* and no reliable data could be generated! */
    /* if event.light is +/- 4294967040 there was a float over/underflow */
    /*
    Serial.println(F("Invalid data (adjust gain or timing)"));
  }
  else
  {
    Serial.print(event.light); Serial.println(F(" lux"));
  }
}


*/
