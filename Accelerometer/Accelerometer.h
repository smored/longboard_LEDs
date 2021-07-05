/* iicInit() --------------------------
This function initializes the iic 
communication with the accelerometer.
It is only necessary to run this once
-------------------------------------*/
void iicInit();

/* iicUpdate() ------------------------
Call this function directly before 
you want to pull data from Acc3D
-------------------------------------*/
Acc3D iicUpdate();

/* Acc3D ------------------------------
This object contains 7 variables:
  double AccVectorSum;
  int16_t AcX, AcY, AcZ;
  bool dirX, dirY, dirZ;
-------------------------------------*/
struct Acc3D;