#include "ESP32_SoftWire.h"

#define _i2c_adr 0x69
const int i2c_SDA_PIN  = 23;
const int i2c_SCL_PIN  = 22;

TwoWire i2c;
TwoWire *_i2c = &i2c;

unsigned int WriteReg( uint8_t reg, uint8_t data )
{
  _i2c->beginTransmission(_i2c_adr); 
  _i2c->write(reg);       
  _i2c->write(data);              
  _i2c->endTransmission();
  return 0;
}

unsigned int ReadReg( uint8_t reg )
{
    uint8_t data = 0;
    ReadRegs(reg, &data, 1);
    return data;
}
void ReadRegs( uint8_t reg, uint8_t *data, uint8_t n )
{
  _i2c->beginTransmission(_i2c_adr); 
  _i2c->write(reg);
  _i2c->endTransmission(false); //false = repeated start
  uint8_t bytesReceived = _i2c->requestFrom(_i2c_adr, n);
  if(bytesReceived == n) {
    _i2c->readBytes(data, bytesReceived);
  }
}

void i2c_scan() {
  Serial.printf("I2C: Scanning ...\n");
  byte count = 0;
  _i2c->begin();
  for (byte i = 8; i < 120; i++) {
    _i2c->beginTransmission(i);          // Begin I2C transmission Address (i)
    if (_i2c->endTransmission() == 0) { // Receive 0 = success (ACK response) 
      Serial.printf("I2C: Found address: 0x%02X (%d)\n",i,i);
      count++;
    }
  }
  Serial.printf("I2C: Found %d device(s)\n", count);      
}


void setup() {
  Serial.begin(115200);
  i2c.begin(i2c_SDA_PIN, i2c_SCL_PIN, 400000);
}

void loop() {
  i2c_scan();

  int whoami = ReadReg(117);
  Serial.printf("whoami=0x%2X (%d)\n",whoami,whoami);

  delay(1000);
}
