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
 #include "application.h"
 #include "SensorBase.h"
 #include "SensorController.h"

 #include <vector>
 #include <string>

 using namespace std;


// Add new sensors here, will automatically update and publicate them using
// overloaded functions.
 void SensorController::SetupSensors()
 {
   uvSensor = VEML6070();
   if(uvSensor.Connect())
   {
     sensorsConnected.push_back(& uvSensor);
   }else{
     errorSensors.push_back(& uvSensor);
   }

   weatherSensor = BME280();
   if(weatherSensor.Connect())
   {
     sensorsConnected.push_back(& weatherSensor);
   }else{
     errorSensors.push_back(& weatherSensor);
   }

   luxSensor = TSL2561();
   if(luxSensor.Connect())
   {
     sensorsConnected.push_back(& luxSensor);
   }else{
     errorSensors.push_back(& luxSensor);
   }

   gasSensor = CCS811();
   if(gasSensor.Connect())
   {
     sensorsConnected.push_back(& gasSensor);
   }else{
     errorSensors.push_back(& gasSensor);
   }

tempSensor = MCP9808_j();
if(tempSensor.Connect())
{
  sensorsConnected.push_back(& tempSensor);
}else{
  errorSensors.push_back(& tempSensor);
}

humiditySensor = HTU21();
if(humiditySensor.Connect())
{
  sensorsConnected.push_back(& humiditySensor);
}else{
  errorSensors.push_back(& humiditySensor);
}

 }

 void SensorController::UpdateSensors()
 {
   data.concat(String("{ "));
   for (size_t i = 0; i < sensorsConnected.size(); i++) {
     sensorsConnected[i]->UpdateReading();
     data.concat(sensorsConnected[i]->getJSONData());
   }
   data.concat("\"Dn\": \"" + Time.format(TIME_FORMAT_ISO8601_FULL) + "\", ");
   data.concat("\"Yr\": " + String(Time.year()) + ", ");
   data.concat("\"Mo\": " + String(Time.month()) + ", ");
   data.concat("\"Da\": " + String(Time.day()) + ", ");
   data.concat("\"Hr\": " + String(Time.hour()) + ", ");
   data.concat("\"Mi\": " + String(Time.minute()) + ", ");
   data.concat("\"Sc\": " + String(Time.second()) + ", ");
   data.concat("\"Zo\": " + String(10) + " }");
 }

 void SensorController::CommunicateSensors()
 {
   for (size_t i = 0; i < sensorsConnected.size(); i++) {
     sensorsConnected[i]->PrintToSerial();
   }

 }

 void SensorController::UploadSensors()
 {
   Serial.println(data);
   Particle.publish("SensorReading",data, 60, PRIVATE);
   delay(100);
   Particle.publish("SensorReadings",data, 60, PRIVATE);
   data = String();
   /*
   String data = String("{ ");
     for (size_t i = 0; i < sensorsConnected.size(); i++) {
       data.concat(sensorsConnected[i]->getJSONData());
     }
     data.concat("\"Da\": " + Time.format(time, TIME_FORMAT_ISO8601_FULL) + " }");
     Serial.println(data);
     */
  // char payload[255];
  // snprintf(payload, sizeof(payload), "{ \"Ct\": %.2f, \"Cc\": %.2f, \"Cv\": %.2f, \"Tl\": %.2f,\"Ht\": %.2f, \"Hh\": %.2f }",
  // CCStemp.getAverage(), CCSco2.getAverage(), CCSvoc.getAverage(), TSLlux.getAverage(), HTUtemp.getAverage(), HTUhumid.getAverage());

  //Particle.publish("SensorReading", SensorInfo);
  //  Particle.publish("SensorReading", payload);
 }


/*

JSON Format


{
  "CCStemp": "{{Ct}}",
  "CCSco2": "{{Cc}}",
  "CCSvoc": "{{Cv}}",
  "TSLlux": "{{Tl}}",
  "TSLir": "{{Ti}}",
  "TSLvis": "{{Tv}}",
  "HTUtemp": "{{Ht}}",
  "HTUhum": "{{Hh}}",
  "BMEtemp": "{{Bt}}",
  "BMEpres": "{{Bp}}",
  "BMEhum": "{{Ba}}",
  "MCPtemp": "{{Mt}}",
  "VEMuv": "{{UV}}",
  "Date": "{{Da}}",
  "device_id": "{{{PARTICLE_DEVICE_ID}}}",
  "published_at": "{{{PARTICLE_PUBLISHED_AT}}}"
}



*/
