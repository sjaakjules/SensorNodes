//=======================================
//  dependency guards
#ifndef SensorController_h
#define SensorController_h

#if defined(PARTICLE)
#include <Particle.h>
#if (SYSTEM_VERSION < 0x00060100)
#error requires system target 0.6.1 or above
#endif
#endif

#if defined(PARTICLE)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#if !defined(PARTICLE)
#include "Wire.h"
#endif

//======================================
//  Forward declared dependencies
class SensorBase;
class BME280;
class VEML6070;

//======================================
//  Dependencies
#include "SensorBase.h"
#include <vector>
#include <string>

//using namespace std;

// Sensors in use
#include "Sensors/VEML6070.h"
#include "Sensors/BME280.h"
#include "Sensors/TSL2561.h"
#include "Sensors/CCS811.h"
#include "Sensors/MCP9808_j.h"
#include "Sensors/HTU21.h"
#include "Sensors/ads1115g.h"
#include "Sensors/ads1115v.h"
#include "Sensors/SGP30.h"

//======================================
//  Defined variables
#define MCP9808_addr 0x18 //Temperature
#define HTU21D_addr 0x40  //humidity
#define MS5637_addr 0x76  //Pressure
#define CCS811_addr 0x5B  //VOCs CO2

#define BME280_addr 0x77 //Temperature, Pressure, Humidity (0x76 SO to gnd)
//#define SGP30_addr 0x58       //VOCs CO2

#define VEML6070_1_addr 0x30 //UV light
#define VEML6070_2_addr 0x39 //UV light
#define TSL2561_addr 0x29    //IR and visual light
#define DS1307_1_addr 0x50   //RTC
#define DS1307_2_addr 0x68   //RTC
#define ADS1115_1_addr 0x48  //analogue expander
#define ADS1115_2_addr 0x49  //analogue expander
#define SGP30_addr 0x38      //Vocs Co2

//======================================
// The Class
class SensorController
{

public:
  SensorController(void){}; //Start controller and initalise std values

  void SetupSensors(); //Setup sensors. update connections.
  bool setGasBase(uint16_t eCO2_baseline, uint16_t TVOC_baseline);
  uint16_t *getGasBase();
  void UpdateSensors();
  void CommunicateSensors();
  void UploadSensors();
  int getNumberOfReadings() { return sensorReadings.size(); };
  double *getLatestData() { return staticReadings[savedReadings - 1]; };
  String getJsonCodes(int index) { return String(JasonName[index]); };

  bool isReadyToSend() { return readyToSend; };
  String ComName;

private:
  std::vector<SensorBase *> sensorsConnected;
  std::vector<SensorBase *> errorSensors;

 // VEML6070 uvSensor;
 // BME280 weatherSensor;
 // TSL2561 luxSensor;
 // CCS811 gasSensor2;
 // MCP9808_j tempSensor;
 // HTU21 humiditySensor;
 // ads1115g analogueSensors1;
 // ads1115v analogueSensors2;
  SGP30 gasSensor;

  const static int nStaticReadings = 13;
  const static int maxSavedReadings = 10;

  std::vector<double *> sensorReadings;
  double staticReadings[maxSavedReadings][nStaticReadings];
  String JasonName[nStaticReadings];

  int totalReadings;
  int savedReadings = 0;
  bool readyToSend = false;

  String data;
};

#endif
