/*
 * Project SensorNode
 * Description: Main script
 * Author:  Julian Rutten - jrutten@swin.edu.au
 * Date:    11/07/2018
 */


#include "SensorController.h"
#include "SensorBase.h"

SensorController sensors;
int counter = 0;

void UploadSensors()
{
    sensors.UploadSensors();
}
void UpdateSensors()
{
  sensors.UpdateSensors();
  sensors.UploadSensors();
}

Timer timer1(15000, UpdateSensors);
//Timer timer2(30000, UploadSensors);

/*
TimerHandle_t timer;

void timerCallback(TimerHandle_t handle)
{
    static int count = 0;
    digitalWrite(D7, !digitalRead(D7));
    Serial.println(count++);
}

void createTimer()
{
    timer = xTimerCreate("timer", 500, true, NULL, timerCallback);
    xTimerStart(timer, 0);
}
*/

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);
  Time.zone(+10);
  sensors.SetupSensors();
  sensors.UpdateSensors();
  timer1.start();
  //timer2.start();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
/*
  sensors.UpdateSensors();
if (counter > 5) {
  sensors.CommunicateSensors();
//  sensors.UploadSensors();
  counter = 0;
}
counter++;
*/
delay(10000);
}
