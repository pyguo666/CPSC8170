#include "Face.h"

Face::Face(){
  for(int i=0; i<3; i++){
    strutCon[i] = -1;
    particleCon[i] = -1;
    vertexAngles[i] = -1;
  }
}

Face::Face(int s[3], int p[3], double v[3]){
  for(int i = 0; i < 3; i++){
    strutCon[i] = s[i];
    particleCon[i] = p[i];
    vertexAngles[i] = v[i];
  }
}
