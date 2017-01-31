/**************************************************
  Particles EveryWhere
  Student Name: Yang Guo
  Assignment02  CPSC8170
 **************************************************/

#include <iostream>
#include "Vector.h"

class  Particle{
 public:
  // To simplify the program, just set all the variables and functions Public

  Particle();
  Particle(Vector3d position1, Vector3d velocity1, Vector4d color1, Vector3d accel1, int life1, double mass1, bool flag1);

  Vector3d position;
  Vector3d velocity;
  Vector4d color;
  Vector3d accel;
  int lifeSpan;
  double mass;
  bool flag;
};
