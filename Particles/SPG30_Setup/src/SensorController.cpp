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
//  Dependencies
#include "application.h"
#include "SensorBase.h"
#include "SensorController.h"

#include <vector>
#include <string>

//using namespace std;

// Add new sensors here, will automatically update and publicate them using
// overloaded functions.
void SensorController::SetupSensors()
{

  gasSensor = SGP30();
  if (gasSensor.Connect())
  {
    sensorsConnected.push_back(&gasSensor);
    totalReadings += gasSensor.getNumberSensors();
  }
  else
  {
    errorSensors.push_back(&gasSensor);
  }
  delay(1000);
}
  //Serial.println("Numbers Added from constructors: " + String(totalReadings));

  
//////////////////////////////////////////////////////////////
//           SENSORS NOT ACTIVE
//////////////////////////////////////////////////////////////
  // The following are not going to be included!!!
  /*

  weatherSensor = BME280();
  if (weatherSensor.Connect())
  {
    sensorsConnected.push_back(&weatherSensor);
    totalReadings += weatherSensor.getNumberSensors();
  }
  else
  {
    errorSensors.push_back(&weatherSensor);
  }
  luxSensor = TSL2561();
  if (luxSensor.Connect())
  {
    sensorsConnected.push_back(&luxSensor);
    totalReadings += luxSensor.getNumberSensors();
  }
  else
  {
    errorSensors.push_back(&luxSensor);
  }
  uvSensor = VEML6070();
  if (uvSensor.Connect())
  {
    sensorsConnected.push_back(&uvSensor);
    totalReadings += uvSensor.getNumberSensors();
  }
  else
  {
    errorSensors.push_back(&uvSensor);
  }

  analogueSensors1 = ads1115g();
  if(analogueSensors1.Connect())
  {
  sensorsConnected.push_back(& analogueSensors1);
  totalReadings += analogueSensors1.getNumberSensors();
}else{
errorSensors.push_back(& analogueSensors1);
}

analogueSensors2 = ads1115v();
if(analogueSensors2.Connect())
{
sensorsConnected.push_back(& analogueSensors2);
totalReadings += analogueSensors2.getNumberSensors();
}else{
errorSensors.push_back(& analogueSensors2);
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
*/


bool SensorController::setGasBase(uint16_t eCO2_baseline, uint16_t TVOC_baseline)
{
  if (!gasSensor.setBaseLine(eCO2_baseline, TVOC_baseline))
  {
    Serial.println("Failed to set baseline readings");
    return false;
  }
  return true;
}

uint16_t *SensorController::getGasBase()
{
  return gasSensor.returnBaseLine();
}

void SensorController::UpdateSensors()
{

  if (savedReadings >= maxSavedReadings)
  {
    savedReadings = 0;
    readyToSend = true;
  }

  //  if (savedReadings == 0) {
  JasonName[0] = "Yr";
  JasonName[1] = "Mo";
  JasonName[2] = "Da";
  JasonName[3] = "Hr";
  JasonName[4] = "Mi";
  JasonName[5] = "Sc";
  JasonName[6] = "Zo";

  int count = 7;
  for (size_t i = 0; i < sensorsConnected.size(); i++)
  {
    //    Serial.println("Updating " + sensorsConnected[i]->getName());
    for (int j = 0; j < sensorsConnected[i]->getNumberSensors(); j++)
    {
      JasonName[count] = (sensorsConnected[i]->getJSONCodes())[j];
      count++;
    }
  }
  for (int i = count; i < nStaticReadings; i++)
  {
    JasonName[i] = "--";
  }

  //  }

  // if first time, setup header info.
  // year, month, day, hour,min, second,
  // for each connected sensor String* getJSONCodes()

  //  uint32_t sensorreadingSize;
  //  uint32_t timeStampReadingSize;
  //sensorReadings.push_back()
  //uint32_t freemem1 = System.freeMemory();
  //  double latestReadingValues[totalReadings + 7];

  //  latestReadingValues[0] = Time.year();
  //  latestReadingValues[1] = Time.month();
  //  latestReadingValues[2] = Time.day();
  //  latestReadingValues[3] = Time.hour();
  //  latestReadingValues[4] = Time.minute();
  //  latestReadingValues[5] = Time.second();
  //latestReadingValues[6] = 10;

  staticReadings[savedReadings][0] = Time.year();
  staticReadings[savedReadings][1] = Time.month();
  staticReadings[savedReadings][2] = Time.day();
  staticReadings[savedReadings][3] = Time.hour();
  staticReadings[savedReadings][4] = Time.minute();
  staticReadings[savedReadings][5] = Time.second();
  staticReadings[savedReadings][6] = 10;

  count = 7;
  for (size_t i = 0; i < sensorsConnected.size(); i++)
  {
    sensorsConnected[i]->UpdateReading();
    //    Serial.println("Updating " + sensorsConnected[i]->getName());
    for (int j = 0; j < sensorsConnected[i]->getNumberSensors(); j++)
    {
      //  latestReadingValues[count] = (sensorsConnected[i]->getLatestValues())[j];
      staticReadings[savedReadings][count] = (sensorsConnected[i]->getLatestValues())[j];
      count++;
    }
  }
  for (size_t i = count; i < nStaticReadings; i++)
  {
    staticReadings[savedReadings][i] = -1;
  }
  Serial.println("Saved " + String(savedReadings) + " Readings. \tMemory free: \t" + String(System.freeMemory()));

  if (savedReadings < maxSavedReadings)
  {
    savedReadings++;
  }

  //  if (savedReadings > 3) {
  //    Serial.println("Ready to Send");
  //    readyToSend = true;
  //  WiFi.on();
  //  WiFi.connect();
  //  }

  //  sensorReadings.push_back(latestReadingValues);

  //  Serial.print(String(sensorReadings.size()) + " Readings taken. \t");
  //  Serial.println("Memory free: \t" + String(System.freeMemory()));

  //  if (sensorReadings.size() >   200) {
  //delete[] *sensorReadings[0];
  //    Serial.println("Deleted array");
  //for (size_t i = 0; i < totalReadings + 7; i++) {
  //    delete (*sensorReadings[0])[i];
  //    Serial.print("Deleted array entry at " + String(i));
  //  }
  //  double** tempPointer = sensorReadings[0];
  //    sensorReadings.erase(sensorReadings.begin());
  //    Serial.println("Deleted");
  //    delay(3);
  //  delete[] (*tempPointer);
  //  Serial.println("Deleted array");
  //  }

  /* -------
  -------
  Get the information to sent ot the cloud.... this is from the Sensors


  data.concat(String("{ "));
  for (size_t i = 0; i < sensorsConnected.size(); i++) {
  sensorsConnected[i]->UpdateReading();
  data.concat(sensorsConnected[i]->getJSONData());
}

data.concat("\"Dv\": \"" + ComName + "\", ");
//data.concat("\"Dn\": \"" + Time.format(TIME_FORMAT_ISO8601_FULL) + "\", ");
data.concat("\"Yr\": " + String(Time.year()) + ", ");
data.concat("\"Mo\": " + String(Time.month()) + ", ");
data.concat("\"Da\": " + String(Time.day()) + ", ");
data.concat("\"Hr\": " + String(Time.hour()) + ", ");
data.concat("\"Mi\": " + String(Time.minute()) + ", ");
data.concat("\"Sc\": " + String(Time.second()) + ", ");
data.concat("\"Zo\": " + String(10) + " }");


-----
*/
}

void SensorController::CommunicateSensors()
{
  Serial.println("Memory used: " + String(System.freeMemory()) + " with " + String(savedReadings) + " number of readings.");

  for (size_t j = 0; j < nStaticReadings; j++)
  {
    Serial.print(String("\"" + JasonName[j] + "\":" + String(staticReadings[savedReadings - 1][j], 1) + ", "));
  }
  Serial.print("\n");
  int msgSent = 0;

  for (size_t k = 0; k < sensorReadings.size(); k++)
  {
    //  Serial.println("Memory in upload   : " +  String(System.freeMemory()));
    String message = "";
    message.concat(String("{ "));

    int count = 7;
    for (size_t i = 0; i < sensorsConnected.size(); i++)
    {
      //  Serial.println("getting Connected sensor" + String(i));
      for (int j = 0; j < sensorsConnected[i]->getNumberSensors(); j++)
      {
        //    Serial.println("getting sensor readig" + String(j));
        message.concat(String("\"" + (sensorsConnected[i]->getJSONCodes())[j] + "\":" + String((sensorReadings[k])[count], 4) + ", "));
        count++;
      }
    }

    //                          ((*sensorReadings[k])[i])
    message.concat("\"Dv\": " + ComName + ", ");
    message.concat("\"Yr\": " + String((sensorReadings[k])[0]) + ", ");
    message.concat("\"Mo\": " + String((sensorReadings[k])[1]) + ", ");
    message.concat("\"Da\": " + String((sensorReadings[k])[2]) + ", ");
    message.concat("\"Hr\": " + String((sensorReadings[k])[3]) + ", ");
    message.concat("\"Mi\": " + String((sensorReadings[k])[4]) + ", ");
    message.concat("\"Sc\": " + String((sensorReadings[k])[5]) + ", ");
    message.concat("\"Zo\": " + String((sensorReadings[k])[6]) + " }");
    if (k == 0)
    {
      //  Serial.println(message);
    }
    if (Particle.connected)
    {
      Particle.publish("GeneralPublish", message, 60, PUBLIC); // WebHook to Google Sheets
      msgSent++;
    }
    //    Serial.println("Sending to the cloud... ");
    //  delete message;
  }

  //vector<double*>::iterator iter;

  //for (iter = sensorReadings.begin(); iter != sensorReadings.end(); iter++) {
  //    delete[] iter;
  //  }
  /*
for (size_t i = 0; i < sensorReadings.size(); i++) {
for (size_t j = 0; j < totalReadings + 7; j++) {

Serial.print(String(sensorReadings[i][j],4) + " : ");
}
Serial.println("");
}

Serial.println("clearing saved data.. ");

*/
  if (msgSent < sensorReadings.size())
  {
    for (int i = 0; i < msgSent; i++)
    {
      sensorReadings.erase(sensorReadings.begin());
    }
  }
  else if (msgSent == sensorReadings.size())
  {
    sensorReadings.clear();
  }
}

void SensorController::UploadSensors()
{
  if (readyToSend)
  {
    savedReadings = maxSavedReadings;
  }

  for (int i = 0; i < savedReadings; i++)
  {
    data = String();
    data.concat(String("{ "));
    for (size_t j = 0; j < nStaticReadings; j++)
    {
      data.concat(String("\"" + JasonName[j] + "\":" + String(staticReadings[i][j], 4) + ", "));
      staticReadings[i][j] = -1;
    }
    data.concat("\"Dv\": " + ComName + "} ");
    if (Particle.connected)
    {
      Particle.publish("Burnley", data, 60, PRIVATE); // WebHook to Google Sheets
      //Particle.publish(ComName,data, 60, PRIVATE);

      unsigned long now = millis();
      while ((millis() - now) <= 1000)
      {
      }
    }
  }
  savedReadings = 0;
  readyToSend = false;
  //  Serial.println("Memory used: " + String(System.freeMemory()) + "before upload.");
  data = String();
  sensorReadings.clear();
}
//sentMessage = true;
//  Serial.println("Pushed to the cloud. " + String(System.freeMemory()));
//  WiFi.off();

//  }
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
