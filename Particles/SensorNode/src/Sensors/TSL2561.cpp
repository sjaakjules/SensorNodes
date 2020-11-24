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
 #include <Wire.h>
#endif

 //======================================
 //  Dependencies

 #include "Sensors/TSL2561.h"
 #include "SensorBase.h"
 #include "SensorController.h"
 #include <vector>
 #include <string>



 using namespace std;

TSL2561::TSL2561()
{
  _name = "TSL2561 Lux/visible/IR Light Sensor";
  _nSensorReadings = 3;
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
        tsl.enableAutoRange(true);
        tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
        UpdateReading();
      }
    }
    return _hasSetup;
  }

  bool TSL2561::UpdateReading()
  {
    sensors_event_t event;
    tsl.getEvent(&event);
    if (event.light)
    {
    latestreading[0]=(event.light);
    }
    uint16_t broadband = 0;
    uint16_t infrared = 0;
    tsl.getLuminosity (&broadband, &infrared);
    latestreading[1]=broadband;
    latestreading[2]=infrared;
    return true;
  }


  void TSL2561::PrintToSerial()
  {
        Serial.println(_name);

        Serial.print("Visible Light = ");
        Serial.print(latestreading[0]);
        Serial.println(" Lux");

        Serial.print("Wide Spectrum Light = ");
        Serial.print(latestreading[1]);
        Serial.println(" Unitless");

        Serial.print("IR Light = ");
        Serial.print(latestreading[2]);
        Serial.println(" Unitless");

        Serial.println();


  }



  /*
  ===================================
    notes and info

    // I2C address options
    #define TSL2561_ADDR_LOW          (0x29)
    #define TSL2561_ADDR_FLOAT        (0x39)    // Default address (pin left floating)
    #define TSL2561_ADDR_HIGH         (0x49)



        TSL2561_INTEGRATIONTIME_13MS      = 0x00,    // 13.7ms
        TSL2561_INTEGRATIONTIME_101MS     = 0x01,    // 101ms
        TSL2561_INTEGRATIONTIME_402MS     = 0x02     // 402ms


            TSL2561_GAIN_1X                   = 0x00,    // No gain
            TSL2561_GAIN_16X                  = 0x10,    // 16x gain

  Functions


      Adafruit_TSL2561_Unified(uint8_t addr, int32_t sensorID = -1);
      boolean begin(void);

      void enableAutoRange(bool enable);
      void setIntegrationTime(tsl2561IntegrationTime_t time);
      void setGain(tsl2561Gain_t gain);
      void getLuminosity (uint16_t *broadband, uint16_t *ir);
      uint32_t calculateLux(uint16_t broadband, uint16_t ir);


  */
