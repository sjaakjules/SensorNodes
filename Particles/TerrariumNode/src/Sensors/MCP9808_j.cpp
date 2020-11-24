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

 #include "Sensors/MCP9808_j.h"
 #include "SensorBase.h"
 #include "SensorController.h"
 #include <vector>
 #include <string>



 using namespace std;

MCP9808_j::MCP9808_j(): SensorBase(nReadings)
{
  _name = "MCP9808 Temperature Sensor";
//  _nSensorReadings = 1;
}

  bool MCP9808_j::Connect()
  {
    if (!_hasSetup) {
      if (! mcp.begin()) {
        Serial.println("Could not find a valid MCP9808 sensor, check wiring!");
        _hasSetup = false;
      }else{
        Serial.println("MCP9808 Connected!");
        _hasSetup = true;

        mcp.setResolution(MCP9808_SLOWEST);
        UpdateReading();
      }
    }
    return _hasSetup;
  }

  bool MCP9808_j::UpdateReading()
  {
    latestreading[0] = mcp.getTemperature();
    return true;
  }


  void MCP9808_j::PrintToSerial()
  {
        Serial.println(_name);

        Serial.print("Temperature = ");
        Serial.print(latestreading[0]);
        Serial.println(" *C");

        Serial.println();


  }



  /*
  ===================================
    notes and info

    // I2C address options

    #define MCP9808_DEFAULT_ADDRESS			0x18 // MCP9808 address can be changed from 0x18 to 0x1F

    // Power modes
    #define MCP9808_CONTINUOUS				0x0100
    #define MCP9808_LOW_POWER				0x0000

    // Resolution/Speed modes
    #define MCP9808_FASTEST					0x00 // +0.5°C resolution 		- 30ms conversion time
    #define MCP9808_FAST 					0x01 // +0.25°C resolution		- 65ms conversion time
    #define MCP9808_SLOW					0x02 // +0.125°C resolution 	- 130ms conversion time
    #define MCP9808_SLOWEST					0x03 // +0.0625°C resolution	- 250ms conversion time

  Functions

  	// Constructor
  	MCP9808(uint8_t addr = MCP9808_DEFAULT_ADDRESS);

  	// Public Functions
  	bool begin();
  	float getTemperature();
  	void setPowerMode(uint16_t mode);
  	void setResolution(uint8_t mode);


  */
