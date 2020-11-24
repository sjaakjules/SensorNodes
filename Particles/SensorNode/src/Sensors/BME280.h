//=======================================
//  dependency guards
#ifndef BME280_h
#define BME280_h


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

#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"

using namespace std;


//======================================
// The Class
class BME280 : public SensorBase
{
public:

  BME280();

  bool Connect();
  bool UpdateReading();
  void PrintToSerial();
  double* getLatestValues() {return latestreading;};
  String getJSONData()
  {
    String data;
    for (size_t i = 0; i < 4; i++) {
      data.concat(String("\"" + JSONCodes[i] + "\":" + String(latestreading[i],4) + ", "));
    }
    return data;
  }
private:

  Adafruit_BME280 bme; // I2C
  double latestreading[4] = {1.0,1.0,1.0,1.0};
  String JSONCodes[4] = {String("Bt"), String("Bp"),String("Ba"),String("Bh")};
};

#endif
