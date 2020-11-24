/*
* //////////////////////////////////////////////////
* //making sense of the Parallax PIR sensor's output
* //////////////////////////////////////////////////
*
* Switches a LED according to the state of the sensors output pin.
* Determines the beginning and end of continuous motion sequences.
*
* @author: Kristian Gohlke / krigoo (_) gmail (_) com / http://krx.at
* @date:   3. September 2006
*
* kr1 (cleft) 2006
* released under a creative commons "Attribution-NonCommercial-ShareAlike 2.0" license
* http://creativecommons.org/licenses/by-nc-sa/2.0/de/
*
*
* The Parallax PIR Sensor is an easy to use digital infrared motion sensor module.
* (http://www.parallax.com/detail.asp?product_id=555-28027)
*
* The sensor's output pin goes to HIGH if motion is present.
* However, even if motion is present it goes to LOW from time to time,
* which might give the impression no motion is present.
* This program deals with this issue by ignoring LOW-phases shorter than a given time,
* assuming continuous motion is present during these phases.
*
*/

SYSTEM_MODE(SEMI_AUTOMATIC);
/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 5;

//the time when the sensor outputs a low impulse
long unsigned int lowIn;

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 500;

boolean lockLow = true;
boolean takeLowTime;
/*DO NOT CHANGE*/ const char* PASS = "4Qiud29-da";

int pirPin = D4;    //the digital pin connected to the PIR sensor's output
int ledPin = D3;

//LEDStatus blinkRed(RGB_COLOR_RED, LED_PATTERN_BLINK);
//LEDStatus blinkGreen(RGB_COLOR_WHITE, LED_PATTERN_BLINK);

/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

////////////////////////////
//LOOP
void loop(){
  delay(1000);
  Serial.println("SENSOR High");
  digitalWrite(ledPin, HIGH);
  delay(30000);
  WiFi.on();
  Serial.println("Connecting WiFi...");
  mycustomScan();
  WiFi.connect();
  //Serial.println("Connecting to Cloud...");                                     ///////////////////// UPDATE
  if (waitFor(WiFi.ready, 5000)) {
    Particle.connect();
  }

  Serial.println("turning off`...");
  goToSleep();
}

void goToSleep() {
//  int min = Time.minute();
//  int seconds = Time.second();
//  int timeTillNext30 = ((min < 30) ? (30 - min) : (60 - min))*60 + 60-seconds;

digitalWrite(ledPin, LOW);
delay(10000);
  Serial.end();
  System.sleep(SLEEP_MODE_DEEP,30);
}

void mycustomScan(){
  bool  hasAddedNewSSID = false;
  WiFiAccessPoint aps[20];
  WiFiAccessPoint Myap[5];
  int foundMy = WiFi.getCredentials(Myap, 5);
  int found = WiFi.scan(aps, 20);
  for (int i=0; i<found; i++) {
    WiFiAccessPoint& ap = aps[i];
    if (ap.ssid[0] == '3' && ap.ssid[1] == 'G' ) {
      bool haveGot = false;

      for (int j = 0; j < foundMy; j++) {
        if (Myap[j].ssid == ap.ssid) {
          haveGot=true;
          break;
        }
      }
      if (!haveGot) {
        Serial.print("SSID: ");
        Serial.println(ap.ssid);
        WiFi.setCredentials(ap.ssid, PASS);
        hasAddedNewSSID = true;
      }else{
        Serial.print("I already know SSID: ");
        Serial.println(ap.ssid);
      }
    }else{

      Serial.print("Unknown SSID: ");
      Serial.println(ap.ssid);
    }

  }
}

/*
if(digitalRead(pirPin) == HIGH){
digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
blinkRed.setActive(true);
if(lockLow){
//makes sure we wait for a transition to LOW before any further output is made:
lockLow = false;
Serial.println("---");
Serial.print("motion detected at ");
Serial.print(millis()/1000);
Serial.println(" sec");
delay(50);
}
takeLowTime = true;
}

if(digitalRead(pirPin) == LOW){
digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
blinkRed.setActive(false);
blinkGreen.setActive(true);
if(takeLowTime){
lowIn = millis();          //save the time of the transition from high to LOW
takeLowTime = false;       //make sure this is only done at the start of a LOW phase
}
//if the sensor is low for more than the given pause,
//we assume that no more motion is going to happen
if(!lockLow && millis() - lowIn > pause){
//makes sure this block of code is only executed again after
//a new motion sequence has been detected
lockLow = true;
Serial.print("motion ended at ");      //output
Serial.print((millis() - pause)/1000);
Serial.println(" sec");
delay(50);
}
}
}
**/
