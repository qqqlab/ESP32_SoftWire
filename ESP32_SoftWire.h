/*==========================================================================================
ESP32_SoftWire

ESP32 Arduino bit banged fast I2C library, drop in replacement for Wire.h

The library reaches up to 3 MHz I2C clock speed. No fancy bits and bobs: no timeouts, no clock stretching, blocking only... Made for fast IMU sensor reading were an occasional missed read does not matter, but bus hangups do matter.

Limitation: pins 0-31 only

Tested on ESP32, might work on other ESP32 variants.

Background

As of December 2023 the official arduino-esp32 Wire library has some nasty bugs. In particular, there is a 1 second timeout which hangs the bus on a bad read, and this timeout can not be lowered without changing the source code, see espressif/esp-idf#4999 and espressif/arduino-esp32#5934. So occasionally (a couple times per minute) 1000 samples are missed when you are reading a sensor at 1 kHz.

MIT License

Copyright (c) 2023 https://github.com/qqqlab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===========================================================================================*/

#ifndef ESP32_SOFTWIRE_H
#define ESP32_SOFTWIRE_H

#include <Arduino.h>

class SoftWire : public Stream {
  public:
    SoftWire();
    //TODO ~SoftWire();

    //call setPins() first, so that begin() can be called without arguments from libraries
    bool setPins(int sda, int scl);

    bool begin(int sda, int scl, uint32_t frequency=0); // returns true, if successful init of i2c bus
    // Explicit Overload for Arduino MainStream API compatibility
    inline bool begin()
    {
        return begin(-1, -1, static_cast<uint32_t>(0));
    }
    bool end();

    //TODO size_t setBufferSize(size_t bSize);

    //TODO void setTimeOut(uint16_t timeOutMillis); // default timeout of i2c transactions is 50ms
    //TODO uint16_t getTimeOut();

    bool setClock(uint32_t frequency);  // Approximate frequency in Hz
    //TODO uint32_t getClock();

    uint8_t beginTransmission(uint8_t address);
    size_t write(uint8_t data);
    uint8_t endTransmission(bool sendStop = true);

    size_t requestFrom(uint8_t address, size_t len, bool stopBit=true) 
    {
      int i;
      _ll_start_cond();
      _ll_write_byte(address<<1 | 1);
      for(i=0; i<len-1; i++) {
        _data[i] = _ll_read_byte(true);
      }
      _data[i++] = _ll_read_byte(false);
      if(stopBit) _ll_stop_cond();
      _data_len = i;
      _data_i = 0;
      return i;
    }
    int available(void) {
      return _data_len - _data_i;
    }
    int read(void) {
      return _data[_data_i++];
    }
    int peek(void) {
      return _data[_data_i];
    } 
    void flush(void) { 
      _data_len = 0;
      _data_i = 0;
    } 

    size_t write(const uint8_t *data, size_t n) 
    {
      size_t cnt = 0;
      for(size_t i=0;i<n;i++) {
        cnt += write(data[i]);
      }
      return cnt;
    }
    inline size_t write(const char * s)
    {
        return write((uint8_t*) s, strlen(s));
    }
    inline size_t write(unsigned long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t)n);
    }
  private:
    uint8_t _sda;
    uint8_t _scl;
    uint32_t _scl_mask;
    uint32_t _sda_mask;
    bool _started;
    uint8_t _data[256];
    uint8_t _data_len;
    uint8_t _data_i;
    uint8_t _ack;

    uint32_t _delay_cycles; //delay half frequency period

    void _ll_start_cond(void);
    void _ll_stop_cond(void);
    void _ll_write_bit(bool bit);
    uint8_t _ll_read_bit(void);
    uint8_t _ll_write_byte(uint8_t byte);
    uint8_t _ll_read_byte(bool ack); 

    void _delay();

};
#endif
