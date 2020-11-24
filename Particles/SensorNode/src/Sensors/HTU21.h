//=======================================
//  dependency guards
#ifndef HTU21_h
#define HTU21_h


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

#include "adafruit-htu21df/adafruit-htu21df.h"

using namespace std;


//======================================
// The Class
class HTU21 : public SensorBase
{
public:

  HTU21();

  bool Connect();
  bool UpdateReading();
  void PrintToSerial();
  double* getLatestValues() {return latestreading;};
  String getJSONData()
  {
    String data;
    for (size_t i = 0; i < 2; i++) {
      data.concat(String("\"" + JSONCodes[i] + "\":" + String(latestreading[i],4) + ", "));
    }
    return data;
  }
private:

  Adafruit_HTU21DF htu = Adafruit_HTU21DF();
  double latestreading[2] = {1.0,1.0};
  String JSONCodes[2] = {String("Ht"), String("Hh")};
};

#endif
