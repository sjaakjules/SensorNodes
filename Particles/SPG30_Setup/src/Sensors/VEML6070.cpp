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

 #include "Sensors/VEML6070.h"
 #include "SensorBase.h"
 #include "SensorController.h"
 #include <vector>
 #include <string>
 #include <math.h>

 //using namespace std;


VEML6070::VEML6070() : SensorBase(nReadings)
{
  _name = "VEML6070 UV Sensor";
  //_nSensorReadings = 1;
}

  bool VEML6070::Connect()
  {
    if (!_hasSetup) {
      uv.begin(VEML6070_4_T);

        Serial.println("VEML6070 connected!");
        _hasSetup = true;
        latestreading[0] =uv.readUV();

    }
    return _hasSetup;
  }

  bool VEML6070::UpdateReading()
  {
    latestreading[0] =uv.readUV();
    return true;
  }


    void VEML6070::PrintToSerial()
    {
          Serial.println(_name);

          Serial.print("UV Light = ");
          Serial.print(latestreading[0] );
          Serial.println(" unitless");

          Serial.println();
    }

    /*
    ===================================
      notes and info

      // really unusual way of getting data, your read from two different addrs!
      #define VEML6070_ADDR_H 0x39
      #define VEML6070_ADDR_L 0x38

    VEML6070_HALF_T ~62.5ms
    VEML6070_1_T ~125ms
    VEML6070_2_T ~250ms
    VEML6070_4_T ~500ms

    Functions
    void begin(veml6070_integrationtime_t itime);
    uint16_t readUV(void);

    */
