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
 #include "Wire.h"
#endif

//======================================
//  Forward declared dependencies

//======================================
//  Dependencies
#include <vector>
#include <string>
#include "SensorBase.h"

#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

//using namespace std;


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
  String* getJSONCodes() {return JSONCodes;};

  String getJSONData()
  {
    String data = String("");
    for (size_t i = 0; i < nReadings; i++) {
      data.concat(String("\"" + JSONCodes[i] + "\":" + String(latestreading[i],4) + ", "));
    }
    return data;
  };
private:

  const static int nReadings = 4;
  Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

  double latestreading[nReadings] = {1.0,1.0,1.0,1.0};
  String JSONCodes[nReadings] = {String("Tl"), String("Tv"),String("Ti"),String("Tf")};
};

#endif
