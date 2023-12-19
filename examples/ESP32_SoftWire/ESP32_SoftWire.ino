#include "ESP32_SoftWire.h"

SoftWire i2c;

const int i2c_SDA_PIN  = 23; // 0-31
const int i2c_SCL_PIN  = 22; // 0-31

struct test_struct{
    uint8_t adr;
    uint8_t reg;
    int expected;
    char descr[40];
};

//table of address, register and expected who_am_i reply
test_struct tests[] = {
  {0x1E, 0x0A, 0x48, "HMC5883L ID-Reg-A 'H'"},
  {0x1E, 0x0B, 0x34, "HMC5883L ID-Reg-B '4'"},
  {0x1E, 0x0C, 0x33, "HMC5883L ID-Reg-C '3'"}, 
  {0x68, 0x75, 0x68, "MPU6050"},
  {0x69, 0x75, 0x68, "MPU6050"},
  {0x68, 0x75, 0x68, "MPU9150"},
  {0x69, 0x75, 0x68, "MPU9150"},  
  {0x68, 0x75, 0x70, "MPU6500"},
  {0x69, 0x75, 0x70, "MPU6500"}, 
  {0x68, 0x75, 0x71, "MPU9250"},
  {0x69, 0x75, 0x71, "MPU9250"},
  {0x76, 0xD0, 0x56, "BMP280"},
  {0x76, 0xD0, 0x57, "BMP280"},    
  {0x76, 0xD0, 0x58, "BMP280"},
  {0x76, 0xD0, 0x60, "BME280"},
  {0x76, 0xD0, 0x61, "BMP680"},  
  {0x77, 0, -1, "MS5611"},
  {0, 0, 0, ""} //end
};

void setup() {
  Serial.begin(115200);
  i2c.begin(i2c_SDA_PIN, i2c_SCL_PIN, 400000);
}

void loop() {
  Serial.println("===================\nESP32 SoftWire Demo\n===================");

  i2c_scan();

  int i = 0;
  while(tests[i].adr) {
    int adr = tests[i].adr;
    int reg = tests[i].reg;
    int expected = tests[i].expected;
    int whoami = i2c_ReadReg(adr, reg);
    Serial.printf("Read from adr 0x%02X reg 0x%02X -> value=0x%02X", adr, reg, whoami);
    if(whoami == expected ) {
      Serial.printf(" ---> MATCHED - ");
    }else{
      Serial.printf(" ---> expected 0x%02X - ", expected);
    }
    Serial.println(tests[i].descr);
    i++;
  }

  delay(1000);
}

void WriteReg( uint8_t adr, uint8_t reg, uint8_t data ) {
  i2c.beginTransmission(adr); 
  i2c.write(reg);       
  i2c.write(data);              
  i2c.endTransmission();
}

unsigned int i2c_ReadReg( uint8_t adr, uint8_t reg ) {
    uint8_t data = 0;
    i2c_ReadRegs(adr, reg, &data, 1);
    return data;
}

void i2c_ReadRegs( uint8_t adr, uint8_t reg, uint8_t *data, uint8_t n ) {
  i2c.beginTransmission(adr); 
  i2c.write(reg);
  i2c.endTransmission(false); //false = repeated start
  uint8_t bytesReceived = i2c.requestFrom(adr, n);
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
