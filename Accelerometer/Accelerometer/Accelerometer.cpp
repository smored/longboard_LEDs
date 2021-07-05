#include <Accelerometer.h>
#include <Wire.h>
#define BASEVALUE 16384.0d
#define GRAVITY 9.81d

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

struct Acc3D {
  double AccVectorSum;
  int16_t AcX, AcY, AcZ;
  bool dirX, dirY, dirZ;
};

Acc3D Acc3D_Board;

void iicInit(){
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

void iicTransmission(){
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
  Acc3D_Board.dirX == AcX>0; 
  Acc3D_Board.dirY == AcY>0;
  Acc3D_Board.dirZ == AcZ>0;

  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | AccVectorSum = "); Serial.println(Acc3D_Board.AccVectorSum);
  
  delay(333);
}
