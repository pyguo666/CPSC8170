/**************************************************
  RBStateDot.cpp
  Student Name: Yang Guo
  Assignment#05  CPSC8170
 **************************************************/
#include "RBStateDot.h"

RBStateDot::RBStateDot():v(Vector3d(0,0,0)),qD(Quaternion()),F(Vector3d(0,0,0)),T(Vector3d(0,0,0))
{

}

RBStateDot RBStateDot::operator*(double h){
  RBStateDot newState;
  newState.v = this->v*h;
  newState.qD = this->qD*h;
  newState.F = this->F*h;
  newState.T = this->T*h;

  return newState;
}

RBStateDot RBStateDot::operator+(RBStateDot state){
  RBStateDot newState;
  newState.v = this->v + state.v;
  newState.qD = this->qD + state.qD;
  newState.F = this->F + state.F;
  newState.T = this->T + state.T;

  return newState;
}
