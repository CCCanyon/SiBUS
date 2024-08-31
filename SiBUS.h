#ifndef SiBUS_H
#define SiBUS_H

#include <Arduino.h>
#include "HardwareSerial.h"
// Arduino Nano 33 IoT (SAMD21) buffer size is 256
#define lost_timeout 200000 // 2 sec -> lost

class SiBUS
{
public:
  SiBUS(HardwareSerial &serial);
  void begin();
  void reset_channels();
  void read();
  bool lost;
  int channel[10];

private:
  HardwareSerial &serialPort;
  int serial_buffer_len = 0;
  byte data[32];
  int sum = 0;
  unsigned long now = 0;
  unsigned long heartbeat = 0; // last good signal time
};

#endif