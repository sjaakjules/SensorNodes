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
#if (SYSTEM_VERSION < 0x00060100)
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

#include "Sensors/SGP30.h"
#include "SensorBase.h"
#include "SensorController.h"
#include <vector>
#include <string>

//using namespace std;

SGP30::SGP30() : SensorBase(nReadings)
{
  _name = "SGP30 VOCs/eCO2 gas Sensor";
  //  _nSensorReadings = 3;
}

bool SGP30::Connect()
{
  if (!_hasSetup)
  {
    if (!sgp.begin())
    {
      Serial.println("Could not find a valid SGP30 sensor, check wiring!");
      _hasSetup = false;
    }
    else
    {
      Serial.println("SGP30 Connected!");
      _hasSetup = true;

      if (!sgp.IAQmeasure())
      {
        Serial.println("Measurement failed");
      }

      uint16_t TVOC_base, eCO2_base;
      for (size_t i = 0; i < 30; i++)
      {
        if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
        {
          Serial.println("Failed to get baseline readings");
        }
      }

      if (!sgp.setIAQBaseline(eCO2_base, TVOC_base))
      {
        Serial.println("Failed to set baseline readings");
      }

      UpdateReading();
    }
  }
  return _hasSetup;
}

bool SGP30::setBaseLine(uint16_t eCO2_baseline, uint16_t TVOC_baseline)
{
  if (!sgp.setIAQBaseline(eCO2_baseline, TVOC_baseline))
  {
    Serial.println("Failed to set baseline readings");
    return false;
  }
  return true;
}

uint16_t *SGP30::returnBaseLine()
{
  uint16_t base[2] = {0, 0};
  uint16_t TVOC_base, eCO2_base;
  if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
  {
    Serial.println("Failed to get baseline readings");
    return base;
  }
  base[0] = eCO2_base;
  base[1] = TVOC_base;
  return base;
}

bool SGP30::UpdateReading()
{
  if (!sgp.IAQmeasure())
  {
    return false;
  }
  latestreading[0] = sgp.eCO2;
  latestreading[1] = sgp.TVOC;

  return true;
}

void SGP30::PrintToSerial()
{
  Serial.println(_name);

  Serial.print("Equivalent CO2 = ");
  Serial.print(latestreading[0]);
  Serial.println(" ppm");

  Serial.print("Total VOCs = ");
  Serial.print(latestreading[1]);
  Serial.println(" ppb");

  Serial.println();
}

/*
===================================
notes and info

// I2C address options

// the i2c address
#define SGP30_I2CADDR_DEFAULT 0x58     ///< SGP30 has only one I2C address


Functions


boolean begin(TwoWire *theWire = NULL);
boolean IAQinit(void);
boolean IAQmeasure(void);

boolean getIAQBaseline(uint16_t *eco2_base, uint16_t *tvoc_base);
boolean setIAQBaseline(uint16_t eco2_base, uint16_t tvoc_base);

The last measurement of the IAQ-calculated Total Volatile Organic Compounds in ppb. This value is set when you call {@link IAQmeasure()}
uint16_t TVOC;


The last measurement of the IAQ-calculated equivalent CO2 in ppm. This value is set when you call {@link IAQmeasure()}
uint16_t eCO2;




*/
