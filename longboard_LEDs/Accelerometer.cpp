#include "Accelerometer.h"
#include <Wire.h>

// bitmask used in readIntStatus
#define STATUS_MASK 0x1

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
Acc3D Acc3D_Board;

void accInit(){
  pinMode(13, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0x8);  // set to 0000 1000 (wakes up the MPU-6050 and disables temperature)
  Wire.endTransmission(true);
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6C); // PWR_MGMT_2 register
  Wire.write(0x7);  // set to 0000 0111 (sets all gyros to standby)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

Acc3D accUpdate(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,6,true);  // request a total of 6 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  Acc3D_Board.AcX = AcX;
  Acc3D_Board.AcY = AcY;
  Acc3D_Board.AcZ = AcZ;
  // Takes absolute magnitude of all 3 axes
  Acc3D_Board.AccVectorSum = sqrt((AcX*AcX)+(AcY*AcY)+(AcZ*AcZ));
  // Shows direction of acceleration (0 is negative direction, 1 is positive)
  Acc3D_Board.dirX = AcX>0; 
  Acc3D_Board.dirY = AcY>0;
  Acc3D_Board.dirZ = AcZ>0;

  // Serial.print("AcX = "); Serial.print(AcX);
  // Serial.print(" | AcY = "); Serial.print(AcY);
  // Serial.print(" | AcZ = "); Serial.print(AcZ);
  // Serial.print(" | AccVectorSum = "); Serial.println(Acc3D_Board.AccVectorSum);
  
  return Acc3D_Board;
}

void accIntInit() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x38); // Write to INT_ENABLE register 
  Wire.write(0x1);  // 0000 0001 = DATA_READY_EN
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x37); // Now we write to the INT_PIN_CFG to flavour our interrupt 
  // We want the following:
  // active high, push-pull, INT held high until clear, 
  // status bit clear on INT_STATUS read, and disable FSYNC
  Wire.write(0x20);  // This means: 0010 0000
  Wire.endTransmission(true);
}

bool accReadIntStatus() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3A);  // Tell it to look at this address
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,1,true);  // request 1 register
  
  return(STATUS_MASK & Wire.read()); //Read the contents and discard unimportant values
}
