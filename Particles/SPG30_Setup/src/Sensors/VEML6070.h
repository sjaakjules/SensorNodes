//=======================================
//  dependency guards
#ifndef VEML6070_h
#define VEML6070_h


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

#include "Adafruit_VEML6070.h"


//using namespace std;

//======================================
// The Class
class VEML6070 : public SensorBase
{
public:

  VEML6070();

  bool Connect();
  bool UpdateReading();
  void PrintToSerial();
  double* getLatestValues() {return latestreading;};
  String* getJSONCodes() {return JSONCodes;};


  String getJSONData()
  {
    String data;
    for (size_t i = 0; i < nReadings; i++) {
      data.concat(String("\"" + JSONCodes[i] + "\":" + String(latestreading[i],4) + ", "));
    }
    return data;
  };

private:
  const static int nReadings = 1;
  Adafruit_VEML6070 uv = Adafruit_VEML6070();
  double latestreading[nReadings] = {1.0};
  String JSONCodes[nReadings] = {String("UV")};
};

#endif
