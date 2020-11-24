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

 #include "Sensors/ads1115g.h"
 #include "SensorBase.h"
 #include "SensorController.h"
 #include <vector>
 #include <string>



 using namespace std;

ads1115g::ads1115g() : SensorBase(nReadings)
{
  _name = "ads1115 Analogue to digital coverter, water, PIR, dust Sensors etc";
//  _nSensorReadings = 4;
}

  bool ads1115g::Connect()
  {
    if (!_hasSetup) {
      //                                                                    ADS1015   ADS1115
      //                                                                    -------   -------
       ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit =       3mV       0.1875mV (DEFAULT)
      // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit =     2mV       0.125mV
      // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit =     1mV       0.0625mV
      // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit =     0.5mV     0.03125mV
      // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit =     0.25mV    0.015625mV
      // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit =     0.125mV   0.0078125mV
      ads.begin();
        Serial.println("ADS1115 Connected!");
        _hasSetup = true;
        UpdateReading();

    }
    return _hasSetup;
  }

  bool ads1115g::UpdateReading()
  {
    double multiplier = 0.1875F;
      short adc0, adc1, adc2, adc3;
    latestreading[0]=(ads.readADC_SingleEnded(0)* multiplier);
    latestreading[1]=(ads.readADC_SingleEnded(1)* multiplier);
    latestreading[2]=(ads.readADC_SingleEnded(2)* multiplier);
    latestreading[3]=(ads.readADC_SingleEnded(3)* multiplier);

    return true;
  }


  void ads1115g::PrintToSerial()
  {
        Serial.println(_name);

        Serial.print("Analogue 1 = ");
        Serial.print(latestreading[0]);
        Serial.println(" v");

        Serial.print("Analogue 2 = ");
        Serial.print(latestreading[1]);
        Serial.println(" v");

        Serial.print("Analogue 3 = ");
        Serial.print(latestreading[2]);
        Serial.println(" v");

        Serial.print("Analogue 4 = ");
        Serial.print(latestreading[3]);
        Serial.println(" v");

        Serial.println();


  }



  /*
  ===================================
    notes and info

  ####  Address info:
0x48 (1001000) ADR -> GND
0x49 (1001001) ADR -> VDD
0x4A (1001010) ADR -> SDA
0x4B (1001011) ADR -> SCL


How it works::
Single Ended inputs measure the voltage between the analog input channel (A0-A3) and analog ground (GND).

Differential inputs measure the voltage between two analog input channels.  (A0&A1 or A2&A3).

  Functions

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful NEVER TO EXCEED +0.3V OVER VDD ON GAINED INPUTS,
  // or exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may DESTROY your ADC!
  //
  //  *** TAKE CARE WHEN SETTING GAIN TO NOT EXCEED ABOVE LIMITS ON INPUT!!
  //                                                                    ADS1015   ADS1115
  //                                                                    -------   -------
   ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit =       3mV       0.1875mV (DEFAULT)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit =     2mV       0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit =     1mV       0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit =     0.5mV     0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit =     0.25mV    0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit =     0.125mV   0.0078125mV

}
Single Ended Conversion:
uint16_t readADC_SingleEnded(uint8_t channel);
Perform a single-ended analog to digital conversion on the specified channel.



Differential Conversion:
int16_t readADC_Differential_0_1(void);
Perform a differential analog to digital conversion on the voltage between channels 0 and 1.

int16_t readADC_Differential_2_3(void);
Perform a differential analog to digital conversion on the voltage between channels 2 and 3.

Comparator Operation:
Comparator mode allows you to compare an input voltage with a threshold level and generate an alert signal (on the ALRT pin) if the threshold is exceeded.  This pin can be polled with a digital input pin, or it can be configured to generate an interrupt.

void startComparator_SingleEnded(uint8_t channel, int16_t threshold);
Set the threshold and channel for comparator operation.

int16_t getLastConversionResults();
Get the last conversion result and clear the comparator.

  */
