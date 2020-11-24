//=======================================
//  dependency guards
#ifndef MCP9808_h
#define MCP9808_h


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

#include "MCP9808.h"

using namespace std;


//======================================
// The Class
class MCP9808_j : public SensorBase
{
public:

  MCP9808_j();

  bool Connect();
  bool UpdateReading();
  void PrintToSerial();
  double* getLatestValues() {return latestreading;};
  String getJSONData()
  {
    String data;
    for (size_t i = 0; i < 1; i++) {
      data.concat(String("\"" + JSONCodes[i] + "\":" + String(latestreading[i],4) + ", "));
    }
    return data;
  };
private:

  MCP9808 mcp = MCP9808();
  double latestreading[1] = {1.0};
  String JSONCodes[1] = {String("Mt")};
};

#endif
