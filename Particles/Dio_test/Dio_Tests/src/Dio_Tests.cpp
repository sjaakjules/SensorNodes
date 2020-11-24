/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "c:/Users/julia/Documents/Particle/projects/Dio_test/Dio_Tests/src/Dio_Tests.ino"
// rf95_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_client
// Tested with Anarduino MiniWirelessLoRa

#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"

#include "Adafruit_SGP30.h"


void setup_Options();
void setup();
void loop();
void publishValues();
void printValues();
void tryConnectWifi();
void tryConnectCloud();
void mycustomScan();
#line 16 "c:/Users/julia/Documents/Particle/projects/Dio_test/Dio_Tests/src/Dio_Tests.ino"
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
Adafruit_SGP30 sgp;
unsigned status;

int wifi_Power = D6;
int SensorPower = D5;
int SignalPower = D4;

retained bool wifiOn = false;

SYSTEM_MODE(SEMI_AUTOMATIC);

STARTUP(setup_Options());
void setup_Options()
{
  //WiFi.selectAntenna(ANT_EXTERNAL);
  System.enableFeature(FEATURE_RETAINED_MEMORY);
 // System.enableFeature(FEATURE_RESET_INFO);
 // System.disableUpdates();
  // System.enableFeature(FEATURE_WIFI_POWERSAVE_CLOCK);
}


//////////////////////////////////////////////////////////////
//            WIFI AND TIMER SETTINGS
//////////////////////////////////////////////////////////////

retained bool hasAddedNewSSID = false;
bool wifiReady = false;
bool cloudReady = false;
bool connectingToCloud = false;

int cloudConnectionDuration = 5000; //ms --will turn off wifi and set isCloudLoop = false
int wifiConnectionDuration = 5000;  //ms --used in waitfor(WiFi.ready)
int wiFiConnectionAttemptMAX = 3;
int wiFiConnectionAttempts = 0;

/*DO NOT CHANGE*/ const char *SSID1 = "3G uFi_C6B";
/*DO NOT CHANGE*/ const char *SSID2 = "3G uFi_9E3";
/*DO NOT CHANGE*/ const char *SSID3 = "3G uFi_C83";
/*DO NOT CHANGE*/ const char *SSID4 = "3G uFi_61B";
/*DO NOT CHANGE*/ const char *SSID5 = "3G uFi_9E0";
/*DO NOT CHANGE*/ const char *PASS = "4Qiud29-da";



void setup() 
{ 
  pinMode(wifi_Power, OUTPUT);  
  pinMode(SensorPower, OUTPUT);
  pinMode(SignalPower, OUTPUT);
  delay(10);
  
  digitalWrite(SignalPower, HIGH);
  Serial.begin(9600);

 // while (!Serial.isConnected())
 //     Particle.process();
  //delay(1000); 
  Log.info("Running Setup for loop");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

   status = bme.begin();

   if (! sgp.begin()){
     Serial.println("Sensor not found :(");
     while (1);
   }
}

void loop()
{
  if (wifiOn)
  {
    digitalWrite(SensorPower, HIGH);
    
    Particle.connect();
    if (Particle.connected())
    {
      
   if (! sgp.IAQmeasure()) {
     Serial.println("Measurement failed");
     return;
   }
   printValues();
   publishValues();
      delay(5000);

      digitalWrite(wifi_Power, LOW);
      digitalWrite(SensorPower, LOW);
      digitalWrite(SignalPower, LOW);
      wifiOn = false;
      System.sleep(SLEEP_MODE_DEEP,  60*3600);
    }
    
  }else
  {
    digitalWrite(wifi_Power, HIGH);
    wifiOn = true;
    System.sleep(D2,CHANGE,30);
  }
  
  
  delay(1000);

}
void publishValues(){

    String message = "";
    message.concat(String("{ "));

      message.concat("\"Bt\":" + String(bme.readTemperature(), 3) + ", ");      
      message.concat("\"Bp\":" + String(bme.readPressure() / 100.0F, 3) + ", ");      
      message.concat("\"Bh\":" + String(bme.readHumidity(), 3) + ", ");      
      message.concat("\"Sv\":" + String(sgp.TVOC) + ", ");      
      message.concat("\"Ss\":" + String(sgp.eCO2) + ", ");    
      message.concat("\"Yr\":" + String(Time.year()) + ", ");
      message.concat("\"Mo\":" + String(Time.month()) + ", ");
      message.concat("\"Da\":" + String(Time.day()) + ", ");
      message.concat("\"Hr\":" + String(Time.hour()) + ", ");
      message.concat("\"Mi\":" + String(Time.minute()) + ", ");
      message.concat("\"Sc\":" + String(Time.second()) + ", ");
      message.concat("\"Zo\":" + String(10) + ", ");

    message.concat("\"Dv\": \"LoRa01\" }");

    Particle.publish("Burnley", String(message), 60, PUBLIC); // WebHook to Google Sheets
    delay(1000);
    Serial.println("Sent to the cloud... ");
    Serial.println(message);

}
void printValues()
{
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
    Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");
    Serial.println();
}

void tryConnectWifi()
{
#if (PLATFORM_ID == PLATFORM_ARGON || PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)
  if (wiFiConnectionAttempts < wiFiConnectionAttemptMAX)
  {
    WiFi.on();
    WiFi.connect();
    Log.info("Connecting WiFi...");
    if (!hasAddedNewSSID)
    {
      mycustomScan();
    }
    Log.info("Connecting to Cloud..."); ///////////////////// UPDATE
    tryConnectCloud();
  }
  else
  {
    //cancelWiFi();
  }
#endif
}

void tryConnectCloud()
{
#if (PLATFORM_ID == PLATFORM_ARGON || PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)

  if (true)
  {
    if ((WiFi.ready() || WiFi.connecting())) //Mesh.ready() || Mesh.connecting())
    {
      if (WiFi.ready())
        Log.info("Wifi is ready.");

      if (WiFi.connecting())
        Log.info("Wifi is connecting.");

      if (waitFor(WiFi.ready, wifiConnectionDuration)) //waitFor(Mesh.ready,wifiConnectionDuration))//
      {
        Log.info("Wifi is ready. now to connect to the cloud. ");
        wifiReady = true;
        connectingToCloud = true;
        Particle.connect();

        if (Particle.connected())
        {
          Log.info("Connected to the cloud... ");
          cloudReady = Particle.connected();
          connectingToCloud = false;
          wiFiConnectionAttempts = 0;
          System.enableUpdates();
          Log.info("All setup, connected and ready to dance. Hazar");
        }
        else
        {
          Log.info("should be connected to cloud... ");
          Particle.connect();
        }
      }
      else
      {
        Log.info("WiFi connection timed out, will try and connect...");
        wiFiConnectionAttempts++;
        tryConnectWifi();
      }
      if (WiFi.connecting())
      {
        Log.info("We are still connecting to the cloud. Will try connect again.");
        wiFiConnectionAttempts++;
        tryConnectWifi();
      }
    }
    else
    {
      Log.info("Is a cloud loop but Wifi is not enabled, will now connect.");

      if (wiFiConnectionAttempts > 1)
        Log.info("Not connected to the cloud. will try again...");

      wiFiConnectionAttempts++;
      tryConnectWifi();
    }
  }
  if (Particle.connected() == false)
  {
    cloudReady = Particle.connected();
    Log.info("Connecting to the cloud... ");
    Particle.connect();
    Log.info("should be connected to cloud... ");
  }
#endif
}

void mycustomScan()
{
  WiFiAccessPoint aps[20];
  WiFiAccessPoint Myap[5];
  int foundMy = WiFi.getCredentials(Myap, 5); //-1;//
  int found = WiFi.scan(aps, 20);             //0;//
  for (int i = 0; i < found; i++)
  {
    WiFiAccessPoint &ap = aps[i];
    if (ap.ssid[0] == '3' && ap.ssid[1] == 'G')
    {
      bool haveGot = false;

      for (int j = 0; j < foundMy; j++)
      {
        if (Myap[j].ssid == ap.ssid)
        {
          haveGot = true;
          break;
        }
      }
      if (!haveGot)
      {
        Serial.print("SSID: ");
        Serial.println(ap.ssid);
        WiFi.setCredentials(ap.ssid, PASS); //
        hasAddedNewSSID = true;
      }
      else
      {
        Serial.print("I already know SSID: ");
        Serial.println(ap.ssid);
      }
    }
    else
    {

      Serial.print("Unknown SSID: ");
      Serial.println(ap.ssid);
    }
  }
}

