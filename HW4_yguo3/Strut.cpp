#include "Strut.h"

Strut::Strut(){
  k = 0;
  d = 0;
  l = 0;
  for(int i = 0; i<2; i++){
    vertexCon[i] = -1;
  }
}

Strut::Strut(double k1, double d1, double l1, int v[2]){
  k = k1;
  d = d1;
  l = l1;
  for(int i = 0; i<2; i++){
    vertexCon[i] = v[i];
  }
}
