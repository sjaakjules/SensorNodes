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
 #include "Wire.h"
#endif

 //======================================
 //  Dependencies

 #include "Sensors/BME280.h"
 #include "SensorBase.h"
 #include "SensorController.h"
 #include <vector>
 #include <string>



 //using namespace std;

BME280::BME280(): SensorBase(nReadings)
{
  _name = "BME280 temperature/pressure/humidy Sensor";
//  _nSensorReadings = 4;
}

  bool BME280::Connect()
  {
    if (!_hasSetup) {
      if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        _hasSetup = false;
      }else{
        Serial.println("BME280 Connected!");
        _hasSetup = true;
        UpdateReading();
      }
    }
    return _hasSetup;
  }

  bool BME280::UpdateReading()
  {

    latestreading[0]=(bme.readTemperature());
    latestreading[1]=(bme.readPressure() / 100.0);
    latestreading[2]=(bme.readAltitude(1013.25));
    latestreading[3]=(bme.readHumidity());

    return true;
  }


  void BME280::PrintToSerial()
  {
        Serial.println(_name);

        Serial.print("Temperature = ");
        Serial.print(latestreading[0]);
        Serial.println(" *C");

        Serial.print("Pressure = ");
        Serial.print(latestreading[1]);
        Serial.println(" hPa");

        Serial.print("Approx. Altitude = ");
        Serial.print(latestreading[2]);
        Serial.println(" m");

        Serial.print("Humidity = ");
        Serial.print(latestreading[3]);
        Serial.println(" percent");

        Serial.println();


  }



  /*
  ===================================
    notes and info

  ####   ADDR of the lib is 0x77
    // Addr of ebay board is 0x76

  Functions

bool  begin(uint8_t addr                  = BME280_ADDRESS); (0x76 it should be)

void takeForcedMeasurement();
float readTemperature(void);
float readPressure(void);
float readHumidity(void);

float readAltitude(float seaLevel);
float seaLevelForAltitude(float altitude, float pressure);

  void setSampling(sensor_mode mode              = MODE_NORMAL,
       sensor_sampling tempSampling  = SAMPLING_X16,
       sensor_sampling pressSampling = SAMPLING_X16,
       sensor_sampling humSampling   = SAMPLING_X16,
       sensor_filter filter          = FILTER_OFF,
       standby_duration duration     = STANDBY_MS_0_5
       );

  enum sensor_sampling {
      SAMPLING_NONE = 0b000,
      SAMPLING_X1   = 0b001,
      SAMPLING_X2   = 0b010,
      SAMPLING_X4   = 0b011,
      SAMPLING_X8   = 0b100,
      SAMPLING_X16  = 0b101
  };

  enum sensor_mode {
      MODE_SLEEP  = 0b00,
      MODE_FORCED = 0b01,
      MODE_NORMAL = 0b11
  };

  enum sensor_filter {
      FILTER_OFF = 0b000,
      FILTER_X2  = 0b001,
      FILTER_X4  = 0b010,
      FILTER_X8  = 0b011,
      FILTER_X16 = 0b100
  };

  // standby durations in ms
  enum standby_duration {
      STANDBY_MS_0_5  = 0b000,
      STANDBY_MS_10   = 0b110,
      STANDBY_MS_20   = 0b111,
      STANDBY_MS_62_5 = 0b001,
      STANDBY_MS_125  = 0b010,
      STANDBY_MS_250  = 0b011,
      STANDBY_MS_500  = 0b100,
      STANDBY_MS_1000 = 0b101
  };

  */
