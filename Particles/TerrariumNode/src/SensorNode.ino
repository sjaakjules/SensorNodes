/*
* Project SensorNode
* Description: Main script
* Author:  Julian Rutten - jrutten@swin.edu.au
* Date:    11/07/2018
*/


#include "SensorController.h"
#include "SensorBase.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
//SYSTEM_THREAD(ENABLED);
STARTUP(setup_Options());

SensorController sensors;
int counter = 0;
String DeviceName = "";
//bool haveGotName = false;
bool isSeadingMessage = false;
bool sentMessage = false;
bool waitingForName = false;
bool isCloudLoop = false;
int sleepDuration = 600;

unsigned long firstAvailable = 0;
bool wifiReady = false;
bool cloudReady = false;
bool connectingToCloud = false;
bool isUsingPowerPack = false;

const int nSaves = 10;
const int nStaticReadings = 18;
//int NPN_Pin = D4;
//int PNP_Pin = D5;

//WIFI CREDENTIALS for development
/*DO NOT CHANGE*/ const char* SSID1 = "3G uFi_61B";
/*DO NOT CHANGE*/ const char* SSID2 = "3G uFi_9E3";
/*DO NOT CHANGE*/ const char* SSID3 = "3G uFi_C83";
/*DO NOT CHANGE*/ const char* SSID4 = "3G uFi_C6B";
/*DO NOT CHANGE*/ const char* SSID5 = "3G uFi_9E0";
/*DO NOT CHANGE*/ const char* PASS = "4Qiud29-da";

retained int nLoops = 0;
retained bool haveGotName = false;
retained char deviceNameRet[25] = "Unknown";
retained int Readings[nSaves][nStaticReadings];
retained char JSONcode[nStaticReadings][3];

//SYSTEM_THREAD(ENABLED);

void setup_Options() {
  WiFi.selectAntenna(ANT_AUTO);
  System.enableFeature(FEATURE_RETAINED_MEMORY);
}

void UploadSensors()
{
  sensors.UploadSensors();
}
void UpdateSensors()
{
  sensors.UpdateSensors();
}


//Timer timer1(10000, UpdateSensors);
//Timer timer2(50000,connectAndUpload);


void handler(const char *topic, const char *data) {
  if (!haveGotName) {
    DeviceName = String(data);
    sensors.ComName = String(data);
    Serial.println("received " + String(topic) + ": " + String(data));
    Serial.println("Saving name...");
    if (DeviceName.length() < 24) {
      for (size_t i = 0; i < DeviceName.length(); i++) {
        deviceNameRet[i] = data[i];
      }
      if (DeviceName.length() < 7) {
        for (size_t i = DeviceName.length(); i < 7; i++) {
          deviceNameRet[i] = '\0' ;
        }
      }
    }else {
      for (size_t i = 0; i < 24; i++) {
        deviceNameRet[i] = data[i];
      }
    }
    haveGotName = true;
    //    Particle.unsubscribe();
    Serial.println("We have a name: " + String(deviceNameRet));
  }
}


// setup() runs once, when the device is first turned on.
void setup() {
/*
  pinMode(NPN_Pin, OUTPUT);
  pinMode(PNP_Pin, OUTPUT);
  delay(10);
  digitalWrite(NPN_Pin,HIGH);
  delay(10);
  digitalWrite(PNP_Pin,HIGH);
  delay(10);
*/

WiFi.setCredentials(SSID1, PASS);
WiFi.setCredentials(SSID2, PASS);
WiFi.setCredentials(SSID3, PASS);
WiFi.setCredentials(SSID4, PASS);
WiFi.setCredentials(SSID5, PASS);
  Serial.begin(9600);
  if (!WiFi.hasCredentials()) {
    Serial.println("No wifi Credentials, setting default now...");
    WiFi.setCredentials(SSID1, PASS);
    WiFi.setCredentials(SSID2, PASS);
    WiFi.setCredentials(SSID3, PASS);
    WiFi.setCredentials(SSID4, PASS);
    WiFi.setCredentials(SSID5, PASS);
  }
  Serial.println("Running Setup for loop, " + String(nLoops));
  if (nLoops % nSaves == 0) {
    isCloudLoop = true;
    WiFi.on();
    Serial.println("Connecting WiFi...");
    WiFi.connect();
    Serial.println("Connecting to Cloud...");                                     ///////////////////// UPDATE
    if (waitFor(WiFi.ready, 5000)) {
      wifiReady = true;
      Particle.connect();
      connectingToCloud = true;
    }
  }
  Serial.println("Running Setup for loop, " + String(nLoops));
  Serial.println("Device name: " + String(deviceNameRet));
  // Put initialization like pinMode and begin functions here.
  waitingForName = haveGotName;

  if(!waitingForName && (nLoops == 0 || !haveGotName) && waitFor(WiFi.ready, 5000) && waitFor(Particle.connected, 5000))
  {
    Serial.println("We dont have the name will register with the cloud...");
    Particle.subscribe("particle/device/name", handler);
    Particle.publish("particle/device/name");
    Serial.println("Waiting to get device name...");
    waitingForName = true;
    cloudReady = true;
  }
  Time.zone(+10);
  Serial.println("Connecting the sensors...");
  sensors.SetupSensors();
  Serial.println("Sensors Connected, now to loop...");
  Serial.println("Time since start is " + String(millis()) + "ms.");

}


// loop() runs over and over again, as quickly as it can execute.
void loop() {
  sensors.UpdateSensors();
  delay(100);
  sensors.UpdateSensors();
  delay(100);
  for (size_t i = 0; i < nSaves; i++) {
    sensors.UpdateSensors();
    saveSensorInfo();
    Serial.println("Finsihed loop " + String(nLoops) + ". Not a cloud loop, so time for sleep...");
    nLoops++;
    delay(500);
  }

  if (isCloudLoop) {
    Serial.println("Loop should be connected to the cloud...");

    if (!wifiReady) {
      Serial.println(WiFi.connecting()?"Connecting to Wifi... stand by...":"Not connecting...");
      if (!WiFi.connecting()) {
        Serial.println("Wifi is not on, or not connecting..");
        WiFi.on();
        Particle.process();
        WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
        Particle.process();
        Serial.println("Now on and connecting...");
      }
    }

    // Connect to the cloud. This happens if we havent already tried to connect.
    if (!connectingToCloud && waitFor(WiFi.ready, 5000)) {
      Serial.println("Starting connection to cloud.");
      wifiReady = true;
      Particle.connect();
      connectingToCloud = true;
    }

    // Once we have connected, or tried to connect, then we ask for our name, if we havnt got it.
    if(connectingToCloud && !waitingForName && (nLoops == 0 || !haveGotName) && waitFor(Particle.connected, 5000))
    {
      Serial.println("We dont have the name will register with the cloud...");
      Particle.subscribe("particle/device/name", handler);
      Particle.publish("particle/device/name");
      Serial.println("Waiting to get device name...");
      waitingForName = true;
      cloudReady = true;
      wifiReady = true;
    }

    if (connectingToCloud && !cloudReady) {
      cloudReady = waitFor(Particle.connected, 5000);
    }

    // If we have our name.. we can upload the data!
    if (cloudReady && haveGotName) {
      Serial.println("Time after connection to cloud " + String(millis() - firstAvailable) + "ms.");

      // we have waited for some time... now to do something...
      Serial.println("loop number is:  " + String(nLoops));
      Serial.println(haveGotName? "I know who i am...":"Who am i?");
      Serial.println("Device name:     " + String(deviceNameRet));
      Serial.println("Loop started at, " + String(firstAvailable));
      Serial.println("Time now is    , " + String(millis() - firstAvailable));

      if (wifiReady && cloudReady) {
        // We are connected to the internet! lets dance!
        Serial.println("We are connected, now to upload!!");
        //  saveSensorInfo();
        //    printSensorInfo();

        Particle.publish("GeneralPublish","Sent Data Now..", 60, PUBLIC);
        publishMyData();
        //delay(5000);


        //  delay(5000);
        Serial.println("Finsihed loop, " + String(nLoops) + "Now time for sleep...");
        //nLoops++;
        if (nLoops >= nSaves) {
          nLoops = 0;
        }

      //  digitalWrite(PNP_Pin,LOW);
      //  digitalWrite(NPN_Pin,LOW);
        Serial.end();
        System.sleep(SLEEP_MODE_DEEP,sleepDuration);

      }
    }
  } else {
    saveSensorInfo();
    //    printSensorInfo();

    //  delay(5000);
    Serial.println("Finsihed loop " + String(nLoops) + ". Not a cloud loop, so time for sleep...");
    nLoops++;
    if (nLoops >= nSaves) {
      nLoops = 0;
    }
  //  digitalWrite(PNP_Pin,LOW);
//    digitalWrite(NPN_Pin,LOW);
    Serial.end();
    System.sleep(SLEEP_MODE_DEEP,sleepDuration);


  }
  delay(1000);
}


void saveSensorInfo() {
  //Serial.printf("Loop %i :",nLoops);
  String tempCode = "--";
  for (size_t i = 0; i < nStaticReadings; i++) {
    Readings[nLoops][i] = (sensors.getLatestData())[i]*100;
    tempCode = String(sensors.getJsonCodes(i));
    JSONcode[i][0] = tempCode.charAt(0);
    JSONcode[i][1] = tempCode.charAt(1);
    JSONcode[i][2] = '\0';
    //Serial.print(tempCode);
    //  Serial.print(" : "+ String(JSONcode[i]));
    //  Serial.print(" : " + String(Readings[nLoops][i] * 1.0f/100));
  }
  //  Serial.println("");
}

void printSensorInfo(){
  for (size_t i = 0; i <= nLoops; i++) {
    Serial.printf("Loop %i :",i);
    for (size_t j = 0; j < nStaticReadings; j++) {
      Serial.print(" : "+ String(JSONcode[j]));
      Serial.print(" : " + String(Readings[i][j] * 1.0f/100));
    }
    Serial.println("");
  }
}


void publishMyData(){
  //  Serial.println("Memory in upload   : " +  String(System.freeMemory()));
  for (size_t j = 0; j < nSaves; j++) {
    String message = "";
    message.concat(String("{ "));

    for (size_t i = 0; i < nStaticReadings; i++)
    {
      message.concat(String("\"" + String(JSONcode[i]) + "\":" + String(Readings[j][i]*1.0f/100,3) + ", "));
    }


    //                          ((*sensorReadings[k])[i])
    message.concat("\"Dv\": \"" + String(deviceNameRet) + "\" }");
    //  message.concat("\"Yr\": " + String((Readings[j])[0]/100) + ", ");
    //  message.concat("\"Mo\": " + String((Readings[j])[1]/100) + ", ");
    //  message.concat("\"Da\": " + String((Readings[j])[2]/100) + ", ");
    //  message.concat("\"Hr\": " + String((Readings[j])[3]/100) + ", ");
    //  message.concat("\"Mi\": " + String((Readings[j])[4]/100) + ", ");
    //  message.concat("\"Sc\": " + String((Readings[j])[5]/100) + ", ");
    //  message.concat("\"Zo\": " + String((Readings[j])[6]/100) + " }");
    Particle.publish("Burnley",String(message), 60, PUBLIC); // WebHook to Google Sheets
    //Particle.publish(ComName,data, 60, PRIVATE);

    delay(1000);
    Serial.println("Sending to the cloud... ");
    Serial.println(message);
    //  delete message;
  }
  nLoops = 0;
}

/*



sensors.UpdateSensors();
for (size_t i = 0; i < 25; i++) {
Readings[nLoops][i] = (sensors.getLatestData())[i]*1000;
JSONcode[nLoops][i][0] = ((sensors.getJsonCodes())[i]).charAt(0);
JSONcode[nLoops][i][1] = ((sensors.getJsonCodes())[i]).charAt(1);
Serial.printf("\t%s : ",JSONcode[nLoops][i]);
Serial.printf("\t%f : ",Readings[nLoops][i] * 1.0f/1000);
}
Serial.println("");
Serial.println("Saved reading, now time to sleep...");
if (wifiReady && cloudReady) {
Serial.println("I am connected... I will upload then time to sleep...");
//  sensors.UploadSensors();                                          ////////////////////////UPDATE
}
nLoops++;
System.sleep(SLEEP_MODE_DEEP,10);
}


delay(1000);
}
/*
if(!haveGotName && WiFi.ready()){
if(waitFor(Particle.connected, 5000))
{
Particle.subscribe("particle/device/name", handler);
Particle.publish("particle/device/name");
Serial.println("Waiting to get device name...");
}
}
while (true) {
delay(40000);
timer1.dispose();
Serial.println("Stopped timer...");
delay(1000);
connectAndUpload();
Serial.println("have uploaded... i think");
timer1.start();
sentMessage = false;
}
}


void CommunicateSensorReadings() {
for (size_t i = 0; i < nLoops; i++) {
data = String();
data.concat(String("{ "));
for (size_t j = 0; j < 25; j++) {
data.concat(String("\"" + JasonName[j] + "\":" + String(staticReadings[i][j],4) + ", "));
staticReadings[i][j] = -1;
}
data.concat("\"Dv\": " + ComName + "} ");
if (Particle.connected) {
Particle.publish("Burnley",data, 60, PRIVATE); // WebHook to Google Sheets
//Particle.publish(ComName,data, 60, PRIVATE);

unsigned long now = millis();
while ((millis()-now) <= 1000) {
}
}
}
savedReadings = 0;
//  Serial.println("Memory used: " + String(System.freeMemory()) + "before upload.");
data = String();
sensorReadings.clear();
}

void connectAndUpload(){
Serial.println("trying to start uploading");
//    WiFi.on();
//  while (!sentMessage) {
//    if(waitFor(sensors.isReadyToSend,1000)){
Serial.println("Ready to send... so lets go!");
if (!WiFi.ready() && !WiFi.connecting()) {
Serial.println("WiFi is turning On");
WiFi.on();
Serial.println("WiFi is On, now connecting");
WiFi.connect();
Serial.println("WiFi is Connecting");
}else {Serial.println("WiFi should be on or connecting");}
if(waitFor(WiFi.ready,1000)){
Serial.println("Particle is Connnecting");
Particle.connect();
Serial.println("Particle is Connnected");
if (waitFor(Particle.connected, 1000)) {
Serial.println("uploading sensors");
sensors.UploadSensors();
//delay(1000);
WiFi.off();
Serial.println("WiFi is Off");
sentMessage = true;
}else {Serial.println("Waiting for particle to connect...");}
}else {Serial.println("Waiting for Wift to be ready...");}
delay(100);
//    }
//  }else {
//    Serial.println("Tried waiting but gave up... ");
//  }
//  }
}

*/





/*
void serialEvent()
{
if (Serial.available() > 0) {
// read the incoming byte:
while (Serial.available() > 0 ) {
char incomingByte = 0; // for incoming serial data
incomingByte = Serial.read();
if (incomingByte != (char)10) {
incommingMsg += String(incomingByte);
}else
{
//    Serial.print("I received a msg: ");
//    Serial.println(String(incommingMsg));
//    incommingMsg = "";
if (isWaitingForCommand) {
if(incommingMsg.equalsIgnoreCase("n") || incommingMsg.equalsIgnoreCase("no")) {

Serial.println("Canceled, please press y to scan.");
isWaitingForPassword = false;
isConfirmingDetails = false;
}
else if (isWaitingForPassword){
if(incommingMsg.equalsIgnoreCase("y") || incommingMsg.equalsIgnoreCase("yes")) {
if (apSelection >= 0 && apSelection < 20) {
WiFiAccessPoint& ap = aps[apSelection];

WiFiCredentials credentials(ap.ssid, ap.);
credentials.setPassword("mypassword")
.setCipher(WLAN_CIPHER_AES);
WiFi.setCredentials(ap.ssid, newPassword, );
}
}
else{
newPassword = String(incommingMsg);
Serial.println("Password recieved was: \'" + incommingMsg + "\'");
Serial.println("Press y to set network detials, retype password or n to cancel.");
}
}
else if(incommingMsg.equalsIgnoreCase("y") || incommingMsg.equalsIgnoreCase("yes")) {

int found = WiFi.scan(aps, 20);
Serial.println("Great found " + String(found) " networks within range.");
Serial.println("Select you network by pressing the number and then enter.");
for (int i=0; i<found; i++) {
WiFiAccessPoint& ap = aps[i];
Serial.print("[" + String(i) + "] SSID: ");
Serial.println(ap.ssid);
}
//  Serial.print("Great. Now please type the SSID of your wifi followed by enter. (The name of the network)");
//isWaitingForCommand = false;
//    isWaitingForSSID = true;
}else if(!isWaitingForPassword && incommingMsg.toInt() >= 0 && incommingMsg.toInt() < 20) {
apSelection = incommingMsg.toInt();
WiFiAccessPoint& ap = aps[apSelection];
Serial.println("Please type password for " + ap.ssid);
isWaitingForPassword = true;
}
else {
Serial.println("Sorry i didn't quite hear you. Please speak up or press y then enter to begin or 'n' to cancel");
}
}
if (isWaitingForSSID) {
newSSID = String(incommingMsg);
isWaitingForSSID = false;
isWaitingForPassword = true;
}
if (isWaitingForPassword) {
newPassword = String(incommingMsg);
isWaitingForPassword = false;
isConfirmingDetails = true;
Serial.println("Just to confirm press 'y' then enter if the following is correct.");
Serial.println("SSID: \t\'" + newSSID  + "\'");
Serial.println("Password: \t\'" + newPassword + "\'");
}
if (isConfirmingDetails) {
if(incommingMsg.equalsIgnoreCase("y") || incommingMsg.equalsIgnoreCase("yes")) {
Serial.print("Great. Just setting the details now.");
WiFi.setCredentials(newSSID, newPassword);
isConfirmingDetails = false;
}else {
Serial.println("Sorry i didn't quite hear you. Please try again later.");
isConfirmingDetails = false;
}
}

incommingMsg = "";
break;
}
}
}
//  char c = Serial.read();
//  Serial.println("Wait what is this?");
//  Serial.print(c);
}

*/
