/*
 * Project SensorNode
 * Description: Main script
 * Author:  Julian Rutten - jrutten@swin.edu.au
 * Date:    11/07/2018
 */

//=======================================
//  dependency guards
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
 //  Dependencies

 #include "Sensors/CCS811.h"
 #include "SensorBase.h"
 #include "SensorController.h"
 #include <vector>
 #include <string>



 using namespace std;

CCS811::CCS811()
{
  _name = "CCS811 VOCs/eCO2 gas Sensor";
  _nSensorReadings = 3;
}

  bool CCS811::Connect()
  {
    if (!_hasSetup) {
      if (!ccs.begin(0x5B)) {
        Serial.println("Could not find a valid CCS811 sensor, check wiring!");
        _hasSetup = false;
      }else{
        Serial.println("CCS811 Connected!");
        _hasSetup = true;

        while(!ccs.available());
        float temp = ccs.calculateTemperature();
        ccs.setTempOffset(temp - 25.0);
        UpdateReading();
      }
    }
    return _hasSetup;
  }

  bool CCS811::UpdateReading()
  {
    if(ccs.available()){
      float temp = ccs.calculateTemperature();
      if(!ccs.readData()){
        latestreading[0] = ccs.geteCO2();
        latestreading[1] =ccs.getTVOC();
        latestreading[2] =temp;
      }
    return true;

  }
}


  void CCS811::PrintToSerial()
  {
        Serial.println(_name);

        Serial.print("Equivalent CO2 = ");
        Serial.print(latestreading[0]);
        Serial.println(" ppm");

        Serial.print("Total VOCs = ");
        Serial.print(latestreading[1]);
        Serial.println(" ppb");

        Serial.print("Temperature = ");
        Serial.print(latestreading[2]);
        Serial.println(" Unitless");

        Serial.println();


  }



  /*
  ===================================
    notes and info

    // I2C address options
      #define CCS811_ADDRESS                (0x5A)
      0x5B is the address


  Functions

  bool begin(uint8_t addr = CCS811_ADDRESS);

  void setEnvironmentalData(uint8_t humidity, double temperature);

  //calculate temperature based on the NTC register
  double calculateTemperature();

  void setThresholds(uint16_t low_med, uint16_t med_high, uint8_t hysteresis = 50);

  void SWReset();

  void setDriveMode(uint8_t mode);
  void enableInterrupt();
  void disableInterrupt();


  uint16_t getTVOC() { return _TVOC; }
  uint16_t geteCO2() { return _eCO2; }

  void setTempOffset(float offset) { _tempOffset = offset; }

  //check if data is available to be read
  bool available();
  uint8_t readData();

  bool checkError();


  */
