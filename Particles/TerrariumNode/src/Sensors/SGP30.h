//=======================================
//  dependency guards
#ifndef SGP30_h
#define SGP30_h


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

#include "Adafruit_SGP30.h"

using namespace std;


//======================================
// The Class
class SGP30 : public SensorBase
{
public:

    SGP30();

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

      const static int nReadings = 2;
       Adafruit_SGP30 sgp;
  double latestreading[nReadings] = {1.0,1.0};
  String JSONCodes[nReadings] = {String("Sc"), String("Sv")};
};

#endif
