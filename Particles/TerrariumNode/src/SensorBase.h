//=======================================
//  dependency guards
#ifndef SensorBase_h
#define SensorBase_h


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
#include "RunningAverage.h"
#include <vector>
#include <string>

using namespace std;


//======================================
// The Class
class SensorBase
{

protected:
  SensorBase(int nValues) { _nSensorReadings = nValues;  };
  //~SensorBase();

  //vector<RunningAverage> Readings;
  //  vector<double> latestReadings;
  String _name;
  int _nSensorReadings;
  bool _hasSetup = false;

public:
  virtual bool Connect() = 0;
  virtual bool UpdateReading() = 0;
  virtual void PrintToSerial() = 0;
  virtual double* getLatestValues() = 0;
  virtual String* getJSONCodes() = 0;
  virtual String getJSONData() = 0;
  //const static int nReadings

  bool isConnected() { return _hasSetup; };
  String getName() { return _name; };
  int  getNumberSensors() {return _nSensorReadings;};

};

#endif
