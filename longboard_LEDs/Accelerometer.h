#include <cstdint>

/* Acc3D ------------------------------
This object contains 7 variables:
  double AccVectorSum;
  int16_t AcX, AcY, AcZ;
  bool dirX, dirY, dirZ;
-------------------------------------*/
struct Acc3D {
  double AccVectorSum;
  int16_t AcX, AcY, AcZ;
  bool dirX, dirY, dirZ;
};



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

/* intInit() --------------------------
This function initializes the data-ready
interrupt on the accelerometer.
This MUST be called after iicInit()
-------------------------------------*/
void intInit();

// probably dont need this
// but it is a thing just in case
//bool readIntStatus();
