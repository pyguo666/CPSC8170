/**************************************************
  Particles EveryWhere
  Student Name: Yang Guo
  Assignment02  CPSC8170
 **************************************************/

#include "Particle.h"

Particle::Particle(){
  position = Vector3d(0,0,0);
  velocity = Vector3d(0,0,0);
  color = Vector4d(1,1,1,1);
  accel = Vector3d(0,0,0);
  lifeSpan = 5;
  mass = 1;
  flag = false;
}

Particle::Particle(Vector3d position1, Vector3d velocity1, Vector4d color1, Vector3d accel1, int life1, double mass1, bool flag1){
  position = position1;
  velocity = velocity1;
  color = color1;
  accel = accel1;
  lifeSpan = life1;
  mass = mass1;
  flag = flag1;
}
