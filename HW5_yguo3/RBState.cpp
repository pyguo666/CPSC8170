/**************************************************
  RBState.cpp
  Student Name: Yang Guo
  Assignment#05  CPSC8170  
 **************************************************/

#include "RBState.h"


RBState::RBState(): x(Vector3d(0,0,0)), q(Quaternion(Matrix3x3(1,0,0, 0,1,0, 0,0,1))), P(Vector3d(0,0,0)),L(Vector3d(0,0,0))
{    
}

RBState RBState::operator+(const RBState state)
{
  RBState newState;

  newState.x = this->x + state.x;
  newState.q = this->q + state.q;
  newState.P = this->P + state.P;
  newState.L = this->L + state.L;
  
  return newState;
}

RBState RBState::operator+(const RBStateDot state)
{
  RBState newState;

  newState.x = this->x + state.v;
  newState.q = this->q + state.qD;
  newState.P = this->P + state.F;
  newState.L = this->L + state.T;
  
  return newState;
}
