/*
 * Project SPG30_Setup
 * Description:
 * Author:
 * Date:
 */
#include "Adafruit_SGP30.h"
//#include "SensorController.h"
//#include "SensorBase.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
STARTUP(setup_Options());

//////////////////////////////////////////////////////////////
//            STRUCTS AND STARTUP
//////////////////////////////////////////////////////////////
struct EEPROMinfo
{
  uint8_t flag;
  char name[24];
  uint16_t TVOC_base, eCO2_base;
  bool setGas;

  EEPROMinfo(String newname) : flag(0), name(""), TVOC_base(0), eCO2_base(0), setGas(false)
  {
    if (newname.length() < 24)
    {
      for (size_t i = 0; i < newname.length(); i++)
      {
        name[i] = newname.charAt(i);
      }
    }
    else
    {
      for (size_t i = 0; i < 24; i++)
      {
        name[i] = newname.charAt(i);
      }
    }
  }

  EEPROMinfo() : flag(0), TVOC_base(0), eCO2_base(0), setGas(false)
  {
    name[0] = '\0';
  }
};

int cloudConnectionDuration = 5000; //ms --will turn off wifi and set isCloudLoop = false
Timer cloudConnectionTimer(5000, cancelWiFiCallback, true);

void setup_Options()
{
  WiFi.selectAntenna(ANT_AUTO);
  System.enableFeature(FEATURE_RETAINED_MEMORY);
  System.enableFeature(FEATURE_RESET_INFO);
  // System.enableFeature(FEATURE_WIFI_POWERSAVE_CLOCK);
}

//////////////////////////////////////////////////////////////
//            THE SAVING SETTINGS
//////////////////////////////////////////////////////////////
bool isDebugMode = false;
bool isCloudLoop = true;
bool forceSave = true;
retained bool isWaitingForBaseValues = true;
const int nSaves = 1;
int minutesToUpdateEEPROM = 1;
int millTillFirstSave = 43200000; // 12 hours

String WebHookName = "Burnley";

//int dongleDelay = 15000; // ms delay after dongle and densors powered up.
//int totalTimeOn = 30000; // ms, total allowed time on before forced shutdown.

//////////////////////////////////////////////////////////////
//           SYSTEM VARIABLES
//////////////////////////////////////////////////////////////
SerialLogHandler logHandler;
//SensorController sensors;
Adafruit_SGP30 sgp;
const int nStaticReadings = 13; // DO NOT EDIT! MUST MATCH SENSORCONTROLLER VALUE
String DeviceName = "";

int nReadingsSaved = 0;
char deviceNameRet[25] = "Unknown";
int Readings[nSaves][nStaticReadings];
retained char JSONcode[nStaticReadings][3];
retained bool hasAddedNewSSID = false;
EEPROMinfo ActiveEEPROMData;
int deviceNumberEEPROMAddress = 0;

bool haveSetup = false;

//////////////////////////////////////////////////////////////
//           SYSTEM TRIGGERS
//////////////////////////////////////////////////////////////
bool waitingForName = false;
bool haveReadEEPROM = false;
bool wifiReady = false;
bool cloudReady = false;
bool connectingToCloud = false;
bool haveGotName = false;

//////////////////////////////////////////////////////////////
//            WIFI AND TIMER SETTINGS
//////////////////////////////////////////////////////////////
int wifiConnectionDuration = 5000; //ms --used in waitfor(WiFi.ready)
int wiFiConnectionAttemptMAX = 3;
//int interval = 60; // interval in minutes to wake with first one at O'clock.

long timeStart = 0;
long timeLoop = 0;
int wiFiConnectionAttempts = 0;
bool haveSavedToCloud = false;

//WIFI CREDENTIALS for development
/*DO NOT CHANGE*/ const char *SSID1 = "3G uFi_61B";
/*DO NOT CHANGE*/ const char *SSID2 = "3G uFi_9E3";
/*DO NOT CHANGE*/ const char *SSID3 = "3G uFi_C83";
/*DO NOT CHANGE*/ const char *SSID4 = "3G uFi_C6B";
/*DO NOT CHANGE*/ const char *SSID5 = "3G uFi_9E0";
/*DO NOT CHANGE*/ const char *PASS = "4Qiud29-da";


//////////////////////////////////////////////////////////////
//            DIO AND SWITCH SETTINGS
//////////////////////////////////////////////////////////////
int i2cDIO = D5;
int wifiDIO = D4;
int motionSensorDIO = D3;

// setup() runs once, when the device is first turned on.
void setup()
{
  Serial.begin(9600);
  if (isDebugMode)
  {
    while (!Serial.isConnected())
      Particle.process();
  }

  pinMode(i2cDIO, OUTPUT);
  pinMode(wifiDIO, OUTPUT);
  pinMode(motionSensorDIO, INPUT_PULLDOWN);

  Log.info("Running Setup for loop, " + String(nReadingsSaved));

  digitalWrite(i2cDIO, HIGH); 
  delay(200);
  digitalWrite(wifiDIO, HIGH); 
  delay(200);

  tryConnectCloud();

  updateName();

  getEEPROMdata();

  if (!sgp.begin())
  {
    Log.error("Could not find a valid SGP30 sensor, check wiring!");
  }
  else
  {
    Log.info("SGP30 Connected!");
  }

  delay(1000);
  //sensors.SetupSensors();

  timeStart = millis();
  timeLoop = millis();
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{

   tryConnectCloud();

  getEEPROMdata();
  updateName();
  trySetupSensor();
  tryMeasure();

  trySaveBase();
  //tryPowerDown();

  delay(500);
}


void trySetupSensor()
{
  if (!haveSetup)
  {
    if (!sgp.IAQinit())
    {
      Log.error("failed to initialize sensor.");
    }
    else
    {
      haveSetup = true;
      Log.info("Sensor is initialized.");
      if (haveReadEEPROM)
      {
        Log.info("Setting Base values from EEPROM.");
        delay(100);
        uint16_t TVOC_base, eCO2_base;
      if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
      {
        Log.error("Failed to get baseline readings");
      }
      else
      {
        Log.info("Sensor Chip base values: \teCO2: " + String(eCO2_base) + " \tTvoc: " + String(TVOC_base));
        delay(100);
      }
      
      if (!sgp.setIAQBaseline(ActiveEEPROMData.eCO2_base, ActiveEEPROMData.TVOC_base))
      {
        Serial.println("Failed to set baseline readings");
      }else{
        Log.info("EEPROM base values: \teCO2: " + String(ActiveEEPROMData.eCO2_base) + " \tTvoc: " + String(ActiveEEPROMData.TVOC_base));
      }
      }
      delay(100);
      if (!sgp.IAQmeasure())
      {
        Log.error("Measurement failed");
      }
      else
      {
        delay(100);
        Log.info("SGP30 reading: \teCO2: " + String(sgp.eCO2) + " \tTvoc: " + String(sgp.TVOC));
      }
    }
  }
}
void tryMeasure()
{
  if (haveSetup)
  {
    if (!sgp.IAQmeasure())
    {
      Log.error("Measurement failed");
    }
    else
    {
      delay(100);
      Log.info("SGP30 reading: " + String(sgp.eCO2) + " : " + String(sgp.TVOC));
    }
  }
}


void updateEEPROM(EEPROMinfo newData)
{
  EEPROMinfo EEPROMdata;
  EEPROM.get(deviceNumberEEPROMAddress, EEPROMdata);

  Log.info("Got info from EEPROM. Now checking.");
  if (EEPROMdata.flag != 0)
  {
    // EEPROM was empty -> initialize myObj

    Log.info("Data in EEPROM is not initialized. Putting new data.");

    EEPROM.put(deviceNumberEEPROMAddress, newData);
    Log.info("Data Put...");
  }
  else
  {
    if (String(newData.name) != String(EEPROMdata.name) || newData.eCO2_base != EEPROMdata.eCO2_base || newData.TVOC_base != EEPROMdata.TVOC_base || newData.setGas !=EEPROMdata.setGas)
    {
      Log.info("Data exists but not the same as current name.");
      Log.info("Name in EEPROM: " + String(EEPROMdata.name));
      Log.info("new name      : " + String(newData.name));
      Log.info("Base values in EEPROM  : " + String(EEPROMdata.eCO2_base) + " : " + String(EEPROMdata.TVOC_base));
      Log.info("Base values in new data: " + String(newData.eCO2_base) + " : " + String(newData.TVOC_base));

      EEPROM.put(deviceNumberEEPROMAddress, newData);
      Log.info("Data Put...");
    }
  }
  Log.info("EEPROM data, " + String(EEPROMdata.flag) + ": " + EEPROMdata.name);
  Log.info("Base values in EEPROM  : " + String(EEPROMdata.eCO2_base) + " : " + String(EEPROMdata.TVOC_base));
  ActiveEEPROMData = newData;
  haveReadEEPROM = true;
}

void trySaveBase()
{
  if (millis() - timeLoop> minutesToUpdateEEPROM*60000)
  {
    isWaitingForBaseValues = true;
    timeLoop = millis();
  }
  
  if (isWaitingForBaseValues && haveReadEEPROM)
  {
    delay(100);
    Log.info("getting Base values");
    uint16_t TVOC_base, eCO2_base;
      if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
      {
        Log.error("Failed to get baseline readings");
      }
      else
      {
        Log.info("Got base values: " + String(eCO2_base) + " : " + String(TVOC_base));
        delay(1000);
      }
    ActiveEEPROMData.eCO2_base = eCO2_base;
    ActiveEEPROMData.TVOC_base = TVOC_base;
    if (forceSave || (millis() - timeStart > millTillFirstSave))
    {
    ActiveEEPROMData.setGas = true;
    updateEEPROM(ActiveEEPROMData);
    timeStart = millis();
    }
    
    isWaitingForBaseValues = false;
  }
}

//////////////////////////////////////////////////////////////
//            SENSOR READING FUNCTIONS
//////////////////////////////////////////////////////////////

void tryPowerDown()
{
  if (!isWaitingForBaseValues)
  {
    resetBools();
    goToSleep();
  }
}

//////////////////////////////////////////////////////////////
//            SYSTEM FUNCTIONS
//////////////////////////////////////////////////////////////

void goToSleep()
{

  if (!isWaitingForBaseValues)
  {

    isWaitingForBaseValues = true;
    Log.info("Sleeping");
    //Serial.end();

#if (PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)
    System.sleep(SLEEP_MODE_DEEP, 43200, SLEEP_DISABLE_WKP_PIN); // 43200
#endif
  }
}
//////////////////////////////////////////////////////////////
//            EEPROM AND DEVICE NAME FUNCTIONS
//////////////////////////////////////////////////////////////
bool getEEPROMdata()
{
  if (!haveReadEEPROM)
  {
    EEPROMinfo EEPROMdata;
    EEPROM.get(deviceNumberEEPROMAddress, EEPROMdata);
    if (EEPROMdata.flag != 0)
    {
      //// EEPROM was empty
      Log.info("EEPROM was empty");
      
      return false;
    }
    ActiveEEPROMData = EEPROMdata;
    haveReadEEPROM = true;
    Log.info("Found EEPROM data, " + String(EEPROMdata.flag) + ": " + EEPROMdata.name);
    return true;
  }
  Log.info("Already loaded EEPROM data. " + String(ActiveEEPROMData.flag) + "\nName: \t" + ActiveEEPROMData.name + "\nSet Gas: \t" + ActiveEEPROMData.setGas + "\nVoC: \t" + ActiveEEPROMData.TVOC_base + "\neCo2: \t" + ActiveEEPROMData.eCO2_base);
  return true;
}

void setNameOfEEPROM(EEPROMinfo newName)
{
  Log.info("NAme to save to EEPROM: " + String(newName.name));
  String Shotname;
  if (String(newName.name).length() > 23)
  {
    Shotname = String(newName.name).substring(0, 22);
  }
  else
  {
    Shotname = String(newName.name);
  }

  Log.info("Name shortened to: " + Shotname);
  EEPROMinfo newData = EEPROMinfo(Shotname);
  if (haveReadEEPROM)
  {
    newData.eCO2_base = ActiveEEPROMData.eCO2_base;
    newData.TVOC_base = ActiveEEPROMData.TVOC_base;
  }
  updateEEPROM(newData);
}

void updateName()
{
  if (!haveGotName)
  {
    Log.info("No name, Will try and get one...");
    /*
    if (getEEPROMdata())
    {
      Log.info("Got name from EEPROM!");
      // got EEPROM data, can update name.
      if (String(ActiveEEPROMData.name).length() == 6)
      {
        setNewName(String(ActiveEEPROMData.name));
      }
    }
    */
    if (isCloudLoop && !haveGotName)
    {
      if (cloudReady && !waitingForName)
      {
        Log.info("We dont have a device name. I will register with the cloud...");
        Particle.subscribe("particle/device/name", handler, ALL_DEVICES);
        Particle.publish("particle/device/name", PUBLIC);
        Log.info("Waiting to get device name...");
        waitingForName = true;
      }
      else if (waitingForName)
      {
        Log.info("Still waiting for a name...");
      }
    }
    else if (!haveGotName)
    {
      Log.info("Not a cloud loop. Checking device for name.");
      if (getEEPROMdata())
      {
        Log.info("Got name from EEPROM!");
        // got EEPROM data, can update name.
        setNewName(String(ActiveEEPROMData.name));
      }
      else
      {
        Log.info("Not a cloud loop and no saved name, I call thee NoName...");
        String tempName = "NoName";
        setNewName(tempName);
        Log.info("We have a name: " + String(deviceNameRet));
      }
    }
  }
}

void setNewName(String newName)
{
  DeviceName = String(newName);
  //sensors.ComName = String(newName);
  if (newName.length() < 24)
  {
    for (size_t i = 0; i < newName.length(); i++)
    {
      deviceNameRet[i] = newName.charAt(i);
    }
    if (DeviceName.length() < 7)
    {
      for (size_t i = DeviceName.length(); i < 7; i++)
      {
        deviceNameRet[i] = '\0';
      }
    }
  }
  else
  {
    for (size_t i = 0; i < 24; i++)
    {
      deviceNameRet[i] = newName.charAt(i);
    }
  }
  haveGotName = true;
}

// This is the call back to get device name.
void handler(const char *topic, const char *data)
{
  Log.info("Call back for device name!");
  if (!haveGotName)
  {
    String loadedNAme = String(data);
    Log.info("received " + String(topic) + ": " + loadedNAme);
    Log.info("Saving name...");

    Log.info("Setting new name...: " + loadedNAme);
    setNewName(loadedNAme);
    Log.info("Set name, now saving to EEPROM...");
    EEPROMinfo Name = EEPROMinfo(loadedNAme);
    setNameOfEEPROM(Name);
    Log.info("We have a name in ram    : " + String(deviceNameRet));
    Log.info("We have a name in on file: " + String(DeviceName));
    Log.info("We have a name in EEPROM : " + String(ActiveEEPROMData.name));
  }
}

//////////////////////////////////////////////////////////////
//            WIFI AND CONNECTION FUNCTIONS
//////////////////////////////////////////////////////////////
void cancelWiFiCallback()
{
  if (connectingToCloud && !cloudReady)
  {
    Log.info("Took too long... will cancel Wifi and the cloud... ");
    cancelWiFi();
  }
}

void cancelWiFi()
{
#if (PLATFORM_ID == PLATFORM_ARGON || PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)
  WiFi.disconnect();
  WiFi.off();
#endif

  isCloudLoop = false;
  wifiReady = false;
  cloudReady = false;
  connectingToCloud = false;
}

void tryConnectWifi()
{
#if (PLATFORM_ID == PLATFORM_ARGON || PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)
  if (wiFiConnectionAttempts < wiFiConnectionAttemptMAX)
  {
    WiFi.on(); //Mesh.on();//
    Log.info("Connecting WiFi...");
    if (!hasAddedNewSSID)
    {
      mycustomScan();
    }
    WiFi.connect();                     //Mesh.connect();//
    Log.info("Connecting to Cloud..."); ///////////////////// UPDATE
    tryConnectCloud();
  }
  else
  {
    cancelWiFi();
  }
#endif
}

void tryConnectCloud()
{
#if (PLATFORM_ID == PLATFORM_ARGON || PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)

  if (isCloudLoop && !cloudReady)
  {
    Log.info("Is a cloud loop but the cloud not connected, will now connect.");
    //Mesh.connect();

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
        cloudConnectionTimer.start();
        connectingToCloud = true;

        Log.info("Connecting to the cloud... ");
        if (Particle.connected() == false)
        {
          Particle.connect();
        }
        if (isCloudLoop)
        {
          cloudReady = Particle.connected();
          connectingToCloud = false;
          wiFiConnectionAttempts = 0;
          Log.info("We are connected to the cloud. Hazar");
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
      Log.info("Not connected to the cloud. will try again...");
      wiFiConnectionAttempts++;
      tryConnectWifi();
    }
  }
#endif
}

#if (PLATFORM_ID == PLATFORM_ARGON || PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)
void Add3GWifi()
{
  WiFi.scan(wifi_scan_callback); //
}

void wifi_scan_callback(WiFiAccessPoint *wap, void *data)
{
  WiFiAccessPoint &ap = *wap;
  if (ap.ssid[0] == '3' && ap.ssid[1] == 'G')
  {
    Serial.print("SSID: ");
    Serial.println(ap.ssid);
    WiFi.setCredentials(ap.ssid, PASS); //
    hasAddedNewSSID = true;
  }
  else
  {

    Serial.print("Unknown SSID: ");
    Serial.println(ap.ssid);
  }
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

#endif

void resetBools()
{

  haveSavedToCloud = false;
  cloudReady = false;
  wifiReady = false;
  connectingToCloud = false;
  haveGotName = false;
}
