/**************************************************
  SpringyMesh.cpp
  Student Name: Yang Guo
  Assignment#04  CPSC8170  
 **************************************************/
//
// Example program to show how to use Chris Root's OpenGL Camera Class

// Christopher Root, 2006
// Minor Modifications by Donald House, 2009
// Minor Modifications by Yujie Shu, 2012

#include "Camera.h"
#include "gauss.h"
#include "Particle.h"
#include "Strut.h"
#include "Face.h"
#include <cmath>
#include <vector>

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define EPS          0.1


int WIDTH = 1000;
int HEIGHT = 800;

const int H = 10;
const int W = 16;
int numParticleWidth = W+1;         //number of particles of flag in the width direction
int numParticleHeight = H+1;
float lengthWidth = 0.5;            //with between particles
float lengthHeight = 0.5;          //height between particles
float lengthCross = sqrt(lengthWidth*lengthWidth+lengthHeight*lengthHeight);
vector<Particle> flagParticles;
vector<Vector3d> forceList;
vector<Strut> struts;
vector<Face> faces;

int persp_win;

Camera *camera;

bool showGrid = true;



// Declaration of important global variables

static Vector3d G(0, -1, 0);
static Vector3d Wind(3,0,0.02);
static double p_mass = 1;
static double TimeStep = 0.01;
static int TimerDelay = 0;  // yaogai
static double accelThreshold = 5;
static float k = 500;
static float d = 20;
static float kC = 20;
static double Cd = 0.6;
static double Cl = 3;


// draws a simple grid
void makeGrid() {
  glColor3f(0.0, 0.0, 0.0);

  glLineWidth(1.0);

  for (float i=-15; i<15; i++) {
    for (float j=-15; j<15; j++) {
      glBegin(GL_LINES);
      glVertex3f(i, 0, j);
      glVertex3f(i, 0, j+1);
      glEnd();
      glBegin(GL_LINES);
      glVertex3f(i, 0, j);
      glVertex3f(i+1, 0, j);
      glEnd();

      if (j == 11){
	glBegin(GL_LINES);
	glVertex3f(i, 0, j+1);
	glVertex3f(i+1, 0, j+1);
	glEnd();
      }
      if (i == 11){
	glBegin(GL_LINES);
	glVertex3f(i+1, 0, j);
	glVertex3f(i+1, 0, j+1);
	glEnd();
      }
    }
  }

  glLineWidth(2.0);
  glBegin(GL_LINES);
  glVertex3f(-15, 0, 0);
  glVertex3f(15, 0, 0);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(0, 0, -15);
  glVertex3f(0, 0, 15);
  glEnd();
  glLineWidth(1.0);
}



/*
   Load parameter file and reinitialize global parameters
*/
void LoadParameters(char *filename){
  
  FILE *paramfile;
  
  if((paramfile = fopen(filename, "r")) == NULL){
    fprintf(stderr, "error opening parameter file %s\n", filename);
    exit(1);
  }
  
  if(fscanf(paramfile, "%lf",
	    &TimeStep) != 1){
    fprintf(stderr, "error reading parameter file %s\n", filename);
    exit(1);
  }
  
  fclose(paramfile);
  
  TimerDelay = int(0.5 * TimeStep * 1000);
}


//This is the class material of CPSC6050 by Geist
void do_lights()
{
float light0_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
float light0_diffuse[] = { 2.0, 2.0, 2.0, 0.0 }; 
float light0_specular[] = { 2.25, 2.25, 2.25, 0.0 }; 
float light0_position[] = { 1.5, 2.0, 2.0, 1.0 };
float light0_direction[] = { -1.5, -2.0, -2.0, 1.0};

// set scene default ambient 
glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient); 

// make specular correct for spots 
glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1); 
glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient); 
glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse); 
glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular); 
glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0); 
glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0); 
glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.0); 
glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.2); 
glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01); 
glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
}




/*
 Initialize the Simulation
 */
void initSimulation(int argc, char* argv[]){

  if(argc != 2){
    fprintf(stderr, "usage: bouncing ball in box param\n");
    exit(1);
  }
  
  LoadParameters(argv[1]);


}
/*

void drawBall(){
  GLfloat ball_mat_ambient[]  = {1.0f, 0.0f, 0.0f, 1.0f};
  GLfloat ball_mat_diffuse[]  = {1.0f, 0.0f, 0.5f, 1.0f};
  GLfloat ball_mat_specular[] = {1.0f, 0.0f, 0.0f, 1.0f};
  GLfloat ball_mat_emission[] = {0.459, 0.361f, 0.231f, 1.0f};
  GLfloat ball_mat_shininess  = 10.0f;
  glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
  glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
  glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
  glPushMatrix();
  //  glColor3f(0.306, 0.18, 0.157);
  glTranslatef(Ball[0], Ball[1], Ball[2]);
  glutSolidSphere(BallRadius, 50, 50);
  glPopMatrix();
}

*/

double calculateAngle(Vector3d &p0, Vector3d &p1, Vector3d &p2){
  Vector3d v1 = p1-p0;
  Vector3d v2 = p2-p0;

  double angle = acos(v1*v2/(v1.norm()*v2.norm()));
  return angle;
}

void particleGenerator(){
  for(int i = 0; i < numParticleHeight; i++){
    for(int j = 0; j < numParticleWidth; j++){
      flagParticles.push_back(Particle(Vector3d(lengthHeight*j, 14 - lengthHeight*i, 0), Vector3d(0,0,0), Vector4d(0.996,0.588,0,0), Vector3d(0,0,0), 1, 1, false));
      forceList.push_back(Vector3d(0,0,0));
    }
  }

  //initiate struts
  for(int i = 0; i < H; i++){
    for(int j = 0; j < W; j++){
      Strut s1;
      Strut s2;
      Strut s3;

      s1.k = k;
      s1.d = d;
      s1.l = lengthWidth;
      s1.vertexCon[0] = i*(W+1) +j;
      s1.vertexCon[1] = i*(W+1) + j +1;

      s2.k = k;
      s2.d = d;
      s2.l = lengthWidth;
      s2.vertexCon[0] = i*(W+1) +j;
      s2.vertexCon[1] = (i+1)*(W+1) + j;

      s3.k = kC;
      s3.d = d/sqrt(2);
      s3.l = lengthCross;
      s3.vertexCon[0] = i*(W+1) +j+1;
      s3.vertexCon[1] = (i+1)*(W+1) + j;

      struts.push_back(s1);
      struts.push_back(s2);
      struts.push_back(s3);
    }
    Strut s4;
    s4.k = k;
    s4.d = d;
    s4.l = lengthWidth;
    s4.vertexCon[0] = i*(W+1) +W;
    s4.vertexCon[1] = (i+1)*(W+1) + W;
    struts.push_back(s4);
  }
  for(int i = 0; i <W; i++){
    Strut s5;
    s5.k = k;
    s5.d = d;
    s5.l = lengthWidth;
    s5.vertexCon[0] = H*(W+1) +i;
    s5.vertexCon[1] = H*(W+1) + i+1;
    struts.push_back(s5);
  }
  
  
  
  //initiate faces

  for(int i = 0; i <H; i++){
    for(int j = 0; j < W; j++){
      Face f1;
      f1.strutCon[0] = i*(3*W+1)+3*j;
      f1.strutCon[1] = i*(3*W+1)+3*j+1;
      f1.strutCon[2] = i*(3*W+1)+3*j+2;
      f1.particleCon[0] = struts[ i*(3*W+1)+3*j].vertexCon[0];
      f1.particleCon[1] = struts[ i*(3*W+1)+3*j+2].vertexCon[0];
      f1.particleCon[2] = struts[ i*(3*W+1)+3*j+2].vertexCon[1];
      f1.vertexAngles[0] = calculateAngle(
                                          flagParticles[struts[ i*(3*W+1)+3*j].vertexCon[0]].position,
                                          flagParticles[struts[ i*(3*W+1)+3*j+2].vertexCon[0]].position,
                                          flagParticles[struts[ i*(3*W+1)+3*j+2].vertexCon[1]].position);
      f1.vertexAngles[1] = calculateAngle(
                                          flagParticles[struts[ i*(3*W+1)+3*j+2].vertexCon[0]].position,
                                          flagParticles[struts[ i*(3*W+1)+3*j].vertexCon[0]].position,
                                          
                                          flagParticles[struts[ i*(3*W+1)+3*j+2].vertexCon[1]].position);
      f1.vertexAngles[2] = calculateAngle(
                                          flagParticles[struts[ i*(3*W+1)+3*j+2].vertexCon[1]].position,
                                          flagParticles[struts[ i*(3*W+1)+3*j].vertexCon[0]].position,
                                          flagParticles[struts[ i*(3*W+1)+3*j+2].vertexCon[0]].position);

      faces.push_back(f1);

      Face f2;
      int tmp1,tmp2,tmp3;
      f2.strutCon[0] = i*(3*W+1) + 3*j + 2;
      tmp1 = i*(3*W+1) + 3*j + 2;
      if(j == W-1){
        f2.strutCon[1] = i*(3*W+1) + 3*W;
        tmp2 =  i*(3*W+1) + 3*W;
      }
      else{
        f2.strutCon[1] = i*(3*W+1) + 3*(j+1) + 1;
        tmp2 =  i*(3*W+1) + 3*(j+1) + 1;
      }
      if(i == H-1){
        f2.strutCon[2] = H*(3*W+1) + j;
        tmp3 =  H*(3*W+1) + j;
      }
      else{
        f2.strutCon[2] = (i+1)*(3*W+1) + 3*j;
        tmp3 = (i+1)*(3*W+1) + 3*j;
      }
      f2.particleCon[0] = struts[tmp1].vertexCon[0];
      f2.particleCon[1] = struts[tmp2].vertexCon[1];
      f2.particleCon[2] = struts[tmp1].vertexCon[1];
      f2.vertexAngles[0] = calculateAngle(
                                          flagParticles[struts[tmp1].vertexCon[0]].position,
                                          flagParticles[struts[tmp2].vertexCon[1]].position,
                                          flagParticles[struts[tmp1].vertexCon[1]].position
                                          );
      f2.vertexAngles[1] = calculateAngle(
                                          flagParticles[struts[tmp2].vertexCon[1]].position,
                                          flagParticles[struts[tmp1].vertexCon[0]].position,
                                          flagParticles[struts[tmp1].vertexCon[1]].position
                                          );
      f2.vertexAngles[2] = calculateAngle(
                                          flagParticles[struts[tmp1].vertexCon[1]].position,
                                          flagParticles[struts[tmp1].vertexCon[0]].position,
                                          flagParticles[struts[tmp2].vertexCon[1]].position
                                          );
      faces.push_back(f2);
    
    }
  }

  
}






void simulate(int){
  int num = flagParticles.size();
  for(int i = 0; i < num; i++){
    forceList[i] = G*p_mass;
  }

  //  cout << "force" << "=:" << forceList[3] <<endl;
 
  // calculate forces between particles
  
  for(int i = 0; i < struts.size(); i++){
    int p1,p2;
    double k,d,l;
    p1 = struts[i].vertexCon[0];
    p2 = struts[i].vertexCon[1];
    k = struts[i].k;
    d = struts[i].d;
    l = struts[i].l;

    Vector3d v1 = flagParticles[p2].position - flagParticles[p1].position;
    Vector3d fs = k*(v1.norm()-l)*v1.normalize();
    Vector3d fd = d*((flagParticles[p2].velocity - flagParticles[p1].velocity)*v1.normalize())*v1.normalize();

    forceList[p1] = forceList[p1]+ fs + fd;
    forceList[p2] = forceList[p2]-fs-fd;
  }

  
  for(int i = 0; i< faces.size(); i++){
    int p1,p2,p3;
    p1 = faces[i].particleCon[0];
    p2 = faces[i].particleCon[1];
    p3 = faces[i].particleCon[2];

    Vector3d v1 = flagParticles[p2].position - flagParticles[p1].position;
    Vector3d v2 = flagParticles[p3].position - flagParticles[p2].position;
    Vector3d n = (v1%v2).normalize();
    Vector3d vRela = ( flagParticles[p1].velocity + flagParticles[p2].velocity + flagParticles[p3].velocity)/3 - Wind;
    Vector3d fd = - Cd*abs(n*vRela)*vRela*(v1%v2).norm()/2;

    Vector3d fl;
    if((n%vRela).norm() < 0.00001){       //adjust
      fl = Vector3d(0,0,0);
    }
    else{
  
      fl = - Cl*(n*vRela)*(vRela%(n%vRela)/(n%vRela).norm())*(v1%v2).norm()/2;
    }


    forceList[p1] = forceList[p1] + (fd+fl)*faces[i].vertexAngles[0]/PI;
    forceList[p2] = forceList[p2] + (fd+fl)*faces[i].vertexAngles[1]/PI;
    forceList[p3] = forceList[p3] + (fd+fl)*faces[i].vertexAngles[2]/PI;
       
  }

  
  for(int i = 0 ; i < numParticleHeight; i++){
    for(int j = 1; j < numParticleWidth; j++){
      
      flagParticles[i*numParticleWidth + j].accel = forceList[i*numParticleWidth + j]/p_mass;
      if( flagParticles[i*numParticleWidth + j].accel.norm() > accelThreshold){
        flagParticles[i*numParticleWidth +j].accel =  flagParticles[i*numParticleWidth + j].accel.normalize()*accelThreshold;
      }
      
      flagParticles[i*numParticleWidth + j].velocity = flagParticles[i*numParticleWidth + j].velocity + flagParticles[i*numParticleWidth + j].accel*TimeStep;
      flagParticles[i*numParticleWidth + j].position = flagParticles[i*numParticleWidth + j].position + flagParticles[i*numParticleWidth + j].velocity*TimeStep;
    }
  }
   
  
  glutPostRedisplay();
  glutTimerFunc(TimerDelay, simulate, 1);
  
}


void init() {
  // set up camera
  // parameters are eye point, aim point, up vector
  camera = new Camera(Vector3d(0, 32, 27), Vector3d(0, 0, 0), 
		      Vector3d(0, 1, 0));

  // grey background for window
  glClearColor(0.44, 0.44, 0.44, 0.0);
  glShadeModel(GL_SMOOTH);
  glDepthRange(0.0, 1.0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);


  particleGenerator();

  /*
  cout << struts.size() << endl;
  cout << faces.size() << endl;
  cout << flagParticles.size() << endl;
  
  cout <<"struts 3:" << struts[9].vertexCon[0] << endl;
  cout <<"struts 3:" << struts[9].vertexCon[1] << endl;
     cout <<"faces  9:" << faces[8].particleCon[0] << endl;
     cout <<"faces  9:" << faces[8].particleCon[1] << endl;
        cout <<"faces  9:" << faces[8].particleCon[2] << endl;
     cout <<"faces  9:" << faces[8].vertexAngles[0] << endl;
     cout <<"faces  9:" << faces[8].vertexAngles[1] << endl;
        cout <<"faces  9:" << faces[8].vertexAngles[2] << endl;
  */
}



void PerspDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw the camera created in perspective
  camera->PerspectiveDisplay(WIDTH, HEIGHT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //
  // here is where you would draw your scene!
  //
  glTranslatef(0, -1, 0);

  if (showGrid){
    glDisable(GL_LIGHTING);
    makeGrid();
  }

  //draw flag pole
  glBegin(GL_LINES);
  glLineWidth(50.0);
  glColor3f(1,1,1);
  glVertex3f(0,0,0);
  glVertex3f(0,14,0);
  glEnd();



  
  do_lights();


  
  
  //draw triangles of the flag
  for(int i = 0; i < numParticleHeight - 1; i++){
    for(int j = 0; j < numParticleWidth - 1; j++){
      if(i%2 == j%2){
        GLfloat ball_mat_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
        GLfloat ball_mat_diffuse[]  = {0, 0, 0, 1.0f};
        GLfloat ball_mat_specular[] = {0, 0, 0, 1.0f};
        GLfloat ball_mat_emission[] = {0, 0, 0, 1.0f};
        GLfloat ball_mat_shininess  = 8.0f;
        
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
        glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
        
      }
      else{
        GLfloat ball_mat_ambient[]  = {1, 1, 1, 1.0f};
        GLfloat ball_mat_diffuse[]  = {1, 1, 1, 1.0f};
        GLfloat ball_mat_specular[] = {1, 1, 1, 1.0f};
        GLfloat ball_mat_emission[] = {1, 1, 1, 1.0f};
        GLfloat ball_mat_shininess  = 8.0f;
        
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
        glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
        
      }
      glBegin(GL_TRIANGLES);
      glVertex3f(flagParticles[i*numParticleWidth+j].position.x,flagParticles[i*numParticleWidth+j].position.y,flagParticles[i*numParticleWidth+j].position.z);
      glVertex3f(flagParticles[i*numParticleWidth+j+1].position.x,flagParticles[i*numParticleWidth+j+1].position.y,flagParticles[i*numParticleWidth+j+1].position.z);
      glVertex3f(flagParticles[(i+1)*numParticleWidth+j].position.x,flagParticles[(i+1)*numParticleWidth+j].position.y,flagParticles[(i+1)*numParticleWidth+j].position.z);
      glEnd();

      glBegin(GL_TRIANGLES);
      glVertex3f(flagParticles[i*numParticleWidth+j+1].position.x,flagParticles[i*numParticleWidth+j+1].position.y,flagParticles[i*numParticleWidth+j+1].position.z);
      glVertex3f(flagParticles[(i+1)*numParticleWidth+j].position.x,flagParticles[(i+1)*numParticleWidth+j].position.y,flagParticles[(i+1)*numParticleWidth+j].position.z);
      glVertex3f(flagParticles[(i+1)*numParticleWidth+j+1].position.x,flagParticles[(i+1)*numParticleWidth+j+1].position.y,flagParticles[(i+1)*numParticleWidth+j+1].position.z);      
      glEnd();
    }
  }
  
  glutSwapBuffers();
}


void mouseEventHandler(int button, int state, int x, int y) {
  // let the camera handle some specific mouse events (similar to maya)
  camera->HandleMouseEvent(button, state, x, y);
  glutPostRedisplay();
}

void motionEventHandler(int x, int y) {
  // let the camera handle some mouse motions if the camera is to be moved
  camera->HandleMouseMotion(x, y);
  glutPostRedisplay();
}

void keyboardEventHandler(unsigned char key, int x, int y) {
  switch (key) {
  case 'r': case 'R':
    // reset the camera to its initial position
    camera->Reset();
    break;
  case 'f': case 'F':
    camera->SetCenterOfFocus(Vector3d(0, 0, 0));
    break;
  case 'g': case 'G':
    showGrid = !showGrid;
    break;
  case 's': case 'S':
    glutTimerFunc(TimerDelay, simulate, 1);
    break;
  case 'q': case 'Q':	// q or esc - quit
    
  case 27:		// esc
    exit(0);
  }

  glutPostRedisplay();
}

int main(int argc, char *argv[]) {

  // set up opengl window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(50, 50);
  persp_win = glutCreateWindow("Waving Flag");

  // initialize the camera and simulation
  init();
  initSimulation(argc, argv);

  // set up opengl callback functions
  glutDisplayFunc(PerspDisplay);
  glutMouseFunc(mouseEventHandler);
  glutMotionFunc(motionEventHandler);
  glutKeyboardFunc(keyboardEventHandler);

  glutMainLoop();
  return(0);
}

