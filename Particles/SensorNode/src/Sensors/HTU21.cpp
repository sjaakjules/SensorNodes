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

 #include "Sensors/HTU21.h"
 #include "SensorBase.h"
 #include "SensorController.h"
 #include <vector>
 #include <string>



 using namespace std;

HTU21::HTU21()
{
  _name = "HTU21D-F Humidity and Temperature Sensor";
  _nSensorReadings = 2;
}

  bool HTU21::Connect()
  {
    if (!_hasSetup) {
      if (!htu.begin()) {
        Serial.println("Could not find a valid HTU21D-F sensor, check wiring!");
        _hasSetup = false;
      }else{
        Serial.println("HTU21D-F Connected!");
        _hasSetup = true;

        UpdateReading();
      }
    }
    return _hasSetup;
  }

  bool HTU21::UpdateReading()
  {
        latestreading[0] = htu.readTemperature();
        latestreading[1] =htu.readHumidity();
    return true;

  }



  void HTU21::PrintToSerial()
  {
        Serial.println(_name);

        Serial.print("Temperature = ");
        Serial.print(latestreading[0]);
        Serial.println(" *C");

        Serial.print("Humidity = ");
        Serial.print(latestreading[1]);
        Serial.println(" %");

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
