//=======================================
//  dependency guards
#ifndef TSL2561_h
#define TSL2561_h


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
//  Forward declared dependencies

//======================================
//  Dependencies
#include <vector>
#include <string>
#include "SensorBase.h"

#include "Adafruit_TSL2561_U.h"

using namespace std;


//======================================
// The Class
class TSL2561 : public SensorBase
{
public:

  TSL2561();

  bool Connect();
  bool UpdateReading();
  void PrintToSerial();
  double* getLatestValues() {return latestreading;};
  String getJSONData()
  {
    String data = String("");
    for (size_t i = 0; i < 3; i++) {
      data.concat(String("\"" + JSONCodes[i] + "\":" + String(latestreading[i],4) + ", "));
    }
    return data;
  }
private:

  Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 12345); // I2C
  double latestreading[3] = {1.0,1.0,1.0};
  String JSONCodes[3] = {String("Tl"), String("Tv"),String("Ti")};
};

#endif
