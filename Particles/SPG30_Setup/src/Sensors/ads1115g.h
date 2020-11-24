//=======================================
//  dependency guards
#ifndef ADS1115_hg
#define ADS1115_hg


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

//#if !defined(ADS1015_ADDRESS)
  #include "Adafruit_ADS1X15.h"

//using namespace std;


//======================================
// The Class
class ads1115g : public SensorBase
{
public:

    ads1115g();

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

      const static int nReadings = 4;
  Adafruit_ADS1115 ads = Adafruit_ADS1115(0x48);  /* Use this for the 16-bit version */
  //Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */
  double latestreading[nReadings] = {1.0,1.0,1.0,1.0};
  String JSONCodes[nReadings] = {String("A1"), String("A2"),String("A3"),String("A4")};
};

#endif
