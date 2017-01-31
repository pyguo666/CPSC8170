/**************************************************
  RBState.h
  Student Name: Yang Guo
  Assignment#05  CPSC8170  
 **************************************************/
#include "Quaternion.h"
#include "Vector.h"
#include "RBStateDot.h"

class RBState {
 public:
  Vector3d x;
  Quaternion q;
  Vector3d P;
  Vector3d L;
    
  RBState();   
  RBState operator+(const RBState state); 
  RBState operator+(const RBStateDot state);    
};


