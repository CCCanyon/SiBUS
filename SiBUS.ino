/*
This is a Simple iBUS reader for FlySky TRX systems.
iBus protocol: https://images.squarespace-cdn.com/content/v1/5bc852d6b9144934c40d499c/1569662017550-NUTP9D3P1QG3WG1CXOBJ/WX20190928-165620%402x.png
Tested on Arduino Nano 33 IoT (SAMD21) with FlySky FS-I6X transmitter and FS-RX2A pro receiver.
Wiring: FS-RX2A IBUS pin to Arduino Nano 33 IoT RX pin.

Features:
* No reading timing requirements, call read() function when you need data.
* Connection lost detection, works only when read() is called frequent enough (within lost detection time, default 2 sec).
* Failsafe to default values when lost.

You can maybe add configurable failsafe values or channel 11 to 14 or many other things to this simple iBus class

*/

#include "SiBUS.h"

#define version F(__FILE__ " " __DATE__ " " __TIME__)

#define LED0 13 // LED lights up when losing signal

SiBUS iBus(Serial1);
// Serial1 using pin RX tested on Arduino Nano 33 IoT, for other boards see https://www.arduino.cc/reference/en/language/functions/communication/serial/


unsigned long dt = 0;
unsigned long then = 0;

unsigned long dbug_timer = 0;
unsigned long dbug_timeout = 25000; // Serial display period here (microsecond)

int saveval=0;

void setup()
{
  pinMode(LED0, OUTPUT);
  Serial.begin(115200);
  delay(500);

  iBus.begin();    // iBUS connected to Serial0 - change to Serial1 or Serial2 port when required

  delay(500);

  Serial.print("\n|version: "); Serial.println(version); // show version

  then = micros();
}

void loop()
{
  // timing for serial display
  dt = micros() - then;
  then += dt;

  iBus.read(); // call when you need an update, it can handle any call frequency
  
  dbug_timer += dt;
  if(dbug_timer > dbug_timeout)
  {
    dbug_timer = 0;
    digitalWrite(LED0, iBus.lost);
    Serial.print("\nDC:");
    Serial.print(iBus.lost);

    for(int i=0; i<4; ++i) // Serial plotter display
    {
      Serial.print(",c");
      Serial.print(i);
      Serial.print(":");
      Serial.print(iBus.channel[i]);
    }

    /*
    for(int i=0; i<10; ++i) // all 10 channel display
    {
      Serial.print("\nch");
      Serial.print(i);
      Serial.print(":");
      Serial.print(iBus.channel[i]);
    }
    */
  }
  delay(100); // change this delay is to see it handle different timings
}
