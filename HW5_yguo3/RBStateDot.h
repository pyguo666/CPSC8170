/**************************************************
  RBStateDot.h
  Student Name: Yang Guo
  Assignment#05  CPSC8170
 **************************************************/
#include "Quaternion.h"
#include "Vector.h"

class RBStateDot{
 public:
  Vector3d v;
  Quaternion qD;
  Vector3d F;
  Vector3d T;

  RBStateDot();

  RBStateDot operator*(double h);
  RBStateDot operator+(RBStateDot state);
};
