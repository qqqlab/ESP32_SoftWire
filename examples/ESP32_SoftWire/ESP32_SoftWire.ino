#include "ESP32_SoftWire.h"

SoftWire i2c;

const int i2c_SDA_PIN  = 23; // 0-31
const int i2c_SCL_PIN  = 22; // 0-31

#define _i2c_adr 0x69 // MPU6050/9150/6500/9250 address

void WriteReg( uint8_t reg, uint8_t data ) {
  i2c.beginTransmission(_i2c_adr); 
  i2c.write(reg);       
  i2c.write(data);              
  i2c.endTransmission();
}

unsigned int i2c_ReadReg( uint8_t reg ) {
    uint8_t data = 0;
    i2c_ReadRegs(reg, &data, 1);
    return data;
}

void i2c_ReadRegs( uint8_t reg, uint8_t *data, uint8_t n ) {
  i2c.beginTransmission(_i2c_adr); 
  i2c.write(reg);
  i2c.endTransmission(false); //false = repeated start
  uint8_t bytesReceived = i2c.requestFrom(_i2c_adr, n);
  if(bytesReceived == n) {
    i2c.readBytes(data, bytesReceived);
  }
}

void i2c_scan() {
  Serial.printf("I2C: Scanning ...\n");
  byte count = 0;
  i2c.begin();
  for (byte i = 8; i < 120; i++) {
    i2c.beginTransmission(i);          // Begin I2C transmission Address (i)
    if (i2c.endTransmission() == 0) { // Receive 0 = success (ACK response) 
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
  Serial.println("===================\nESP32 SoftWire Demo\n===================");

  i2c_scan();

  int whoami = i2c_ReadReg(117);
  Serial.printf("MPU whoami: 0x%2X (%d)\n",whoami,whoami);

  delay(1000);
}
