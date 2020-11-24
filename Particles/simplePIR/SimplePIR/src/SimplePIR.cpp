/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "c:/Users/julia/Documents/Particle/projects/simplePIR/SimplePIR/src/SimplePIR.ino"
/*
 * Project SimplePIR
 * Description:
 * Author:
 * Date:
 */

void setup();
void loop();
#line 8 "c:/Users/julia/Documents/Particle/projects/simplePIR/SimplePIR/src/SimplePIR.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

int pLeftPIR = D0;
int pRightPIR = D1;
int pBoth = D2;

int right, left, both;
// setup() runs once, when the device is first turned on.
void setup()
{
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);
  pinMode(pRightPIR, INPUT);
  pinMode(pLeftPIR, INPUT);
  pinMode(pBoth, INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
  // The core of your code will likely live here.
  right = digitalRead(pRightPIR);  delay(10);
  left = digitalRead(pLeftPIR);  delay(10);
  both = digitalRead(pBoth);  delay(10);
  Serial.println("Right: "+ String(right) + "\tLeft: " + String(left)+"\tBoth: "+ String(both));
}