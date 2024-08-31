#include "SiBUS.h"

SiBUS::SiBUS(HardwareSerial &serial)
  : serialPort(serial)
{
  reset_channels();
}

void SiBUS::reset_channels()
{
  // set default
  channel[0] = 1500;
  channel[1] = 1500;
  channel[2] = 910;
  channel[3] = 1500;
  channel[4] = 1000;
  channel[5] = 1000;
  channel[6] = 1000;
  channel[7] = 1000;
  channel[8] = 1000;
  channel[9] = 1000;
}

void SiBUS::begin()
{
  serialPort.begin(115200);
}

void SiBUS::read()
{
  now = micros();
  if(now - heartbeat < 3000) // if check too soon;
  {
    return;
  }
  else if(now - heartbeat > lost_timeout) // lost
  {
    lost = 1; heartbeat = now - lost_timeout;
    reset_channels(); // failsafe
  }

  serial_buffer_len = serialPort.available(); // available is slow, do it once

  if(serial_buffer_len < 32) // mid-transmission
  {
    //Serial.print("\nerr.1\n");
    return;
  }

  while(serial_buffer_len-- > 63) // trim serial buffer to 63 bytes
  {
    serialPort.read();
  }

  data[0] = serialPort.read();
  data[1] = serialPort.read();
  while(serial_buffer_len)
  {
    if(data[0] == 0x20 && data[1]== 0x40) // find the header
    {
      sum = 0x60; // reset sum = 0x20 + 0x40
      for(int i=2; i<32; ++i)
      {
        data[i] = serialPort.read(); // read the remaining 30 bytes
        sum += data[i]; // sum
      }
      // sum = byte[0] + ... + byte[32]
      // checksum = byte[0] + ... + byte[29] + (byte[30] + (byte[31] << 8)) = 0xFFFF
      // -> sum + byte[31] * (2^8 -1) = 0xFFFF
      if( sum + int(data[31])*0xFF == 0xFFFF) // check the sum
      {
        // update channels
        channel[0] = data[2] + ( (data[3] & 0x0F) << 8 );
        channel[1] = data[4] + ( (data[5] & 0x0F) << 8 );
        channel[2] = data[6] + ( (data[7] & 0x0F) << 8 );
        channel[3] = data[8] + ( (data[9] & 0x0F) << 8 );
        channel[4] = data[10] + ( (data[11] & 0x0F) << 8 );
        channel[5] = data[12] + ( (data[13] & 0x0F) << 8 );
        channel[6] = data[14] + ( (data[15] & 0x0F) << 8 );
        channel[7] = data[16] + ( (data[17] & 0x0F) << 8 );
        channel[8] = data[18] + ( (data[19] & 0x0F) << 8 );
        channel[9] = data[20] + ( (data[21] & 0x0F) << 8 );
        heartbeat = now; // one good packet
        lost = 0; // not lost
      }
      else //
      {
        Serial.print("\nerr.3=");
        Serial.println(sum); // bad checksum
      }
      return;
    }
    // iterate
    data[0] = data[1];
    data[1] = serialPort.read();
    --serial_buffer_len;
  }
}