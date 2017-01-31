/**************************************************
  RigidBody.cpp
  Student Name: Yang Guo
  Assignment#05  CPSC8170  
 **************************************************/
//
// Example program to show how to use Chris Root's OpenGL Camera Class

// Christopher Root, 2006
// Minor Modifications by Donald House, 2009
// Minor Modifications by Yujie Shu, 2012

#include "Camera.h"
#include "gauss.h"
#include "Quaternion.h"
#include "RBState.h"
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


int persp_win;

Camera *camera;

bool showGrid = true;

Vector3d vertexCube[8];
Vector3d vertexForce[8];
Vector3d bodyCenter(0,8,0);
float lengthEdge = 4;

bool cubeMoving = false;
bool changeColor = false;

const float mass = 1;
Matrix3x3 I0(mass/12*(lengthEdge*lengthEdge*2),0,0, 0,mass/12*(lengthEdge*lengthEdge*2),0, 0,0,mass/12*(lengthEdge*lengthEdge*2));

RBState rbState;
RBStateDot rbStateDot;


// Declaration of important global variables

static Vector3d G(0, -2, 0);
static Vector3d Ball(-9,2,-2);
static Vector3d BallVelocity(7,8,2);
static float BallRadius = 0.3;
static double TimeStep = 0.01;
static int TimerDelay = 1;  // yaogai

double errorTh = 0.01;



// draws a simple grid
void makeGrid() {
  GLfloat g_mat_ambient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
  GLfloat g_mat_diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
  GLfloat g_mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
  GLfloat g_mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
  GLfloat g_mat_shininess  = 10.0f;
    
  glMaterialfv(GL_FRONT, GL_AMBIENT,   g_mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   g_mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  g_mat_specular);
  glMaterialfv(GL_FRONT, GL_EMISSION,  g_mat_emission);
  glMaterialf (GL_FRONT, GL_SHININESS, g_mat_shininess);

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

*/


void do_lights()
{
  float light0_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
  float light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; 
  float light0_specular[] = { 1.25, 1.25, 1.25, 1.0 }; 
  float light0_position[] = { 25, 25, 25, 1.0 };
  //  float light0_direction[] = { -1.5, -2.0, -2.0, 1.0};
  float light1_position[] = { 25,0,0, 1.0 };
  // float light1_direction[] = { -1.5, -2.0, -2.0, 1.0};
  float light2_position[] = { -25,-25,-25, 1.0 };
  
  // set scene default ambient 
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient); 

  // make specular correct for spots 
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);

  glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
  // glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);

  glLightfv(GL_LIGHT1, GL_AMBIENT,  light0_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE,  light0_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT1,GL_POSITION,light1_position);
  // glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light1_direction);

    glLightfv(GL_LIGHT2, GL_AMBIENT,  light0_ambient);
  glLightfv(GL_LIGHT2, GL_DIFFUSE,  light0_diffuse);
  glLightfv(GL_LIGHT2, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT2,GL_POSITION,light2_position);
  // glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light1_direction);
 
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
}




/*
 Initialize the Simulation
 */
void initSimulation(int argc, char* argv[]){

  if(argc != 2){
    fprintf(stderr, "usage: Rigid Body\n");
    exit(1);
  }
  
  //LoadParameters(argv[1]);


}


void drawBall(){
  if(changeColor){
    GLfloat cube_mat_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat cube_mat_diffuse[]  = {0.28f, 0.78f, 0.94f, 1.0f};
    GLfloat cube_mat_specular[] = {0.3f, 0.84f, 0.98f, 1.0f};
    GLfloat cube_mat_emission[] = {0.3f, 0.6f, 0.7f, 1.0f};
    GLfloat cube_mat_shininess  = 10.0f;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   cube_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   cube_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  cube_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  cube_mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, cube_mat_shininess);
  }
  else if(!changeColor){
    GLfloat ball_mat_ambient[]  = {0.1f, 0.0f, 0.0f, 1.0f};
    GLfloat ball_mat_diffuse[]  = {1.0f, 0.2f, 0.2f, 1.0f};
    GLfloat ball_mat_specular[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat ball_mat_emission[] = {0.9, 0.2f, 0.2f, 1.0f};
    GLfloat ball_mat_shininess  = 10.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);   
  }
  glPushMatrix();
  //  glColor3f(0.306, 0.18, 0.157);
  glTranslatef(Ball[0], Ball[1], Ball[2]);
  glutSolidSphere(BallRadius, 50, 50);
  glPopMatrix();
}

void drawCube(){
  if(!changeColor){
    GLfloat cube_mat_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat cube_mat_diffuse[]  = {0.28f, 0.78f, 0.94f, 1.0f};
    GLfloat cube_mat_specular[] = {0.3f, 0.84f, 0.98f, 1.0f};
    GLfloat cube_mat_emission[] = {0.3f, 0.6f, 0.7f, 1.0f};
    GLfloat cube_mat_shininess  = 10.0f;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   cube_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   cube_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  cube_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  cube_mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, cube_mat_shininess);
  }
  else if(changeColor){
    GLfloat ball_mat_ambient[]  = {0.1f, 0.0f, 0.0f, 1.0f};
    GLfloat ball_mat_diffuse[]  = {1.0f, 0.2f, 0.2f, 1.0f};
    GLfloat ball_mat_specular[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat ball_mat_emission[] = {0.9, 0.2f, 0.2f, 1.0f};
    GLfloat ball_mat_shininess  = 10.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);   
  }
  glPushMatrix();
  glBegin(GL_POLYGON);     
  glVertex3f(vertexCube[0].x,vertexCube[0].y,vertexCube[0].z);
  glVertex3f(vertexCube[1].x,vertexCube[1].y,vertexCube[1].z);
  glVertex3f(vertexCube[2].x,vertexCube[2].y,vertexCube[2].z);
  glVertex3f(vertexCube[3].x,vertexCube[3].y,vertexCube[3].z);
  glEnd();

  glBegin(GL_POLYGON);     
  glVertex3f(vertexCube[0].x,vertexCube[0].y,vertexCube[0].z);
  glVertex3f(vertexCube[1].x,vertexCube[1].y,vertexCube[1].z);
  glVertex3f(vertexCube[5].x,vertexCube[5].y,vertexCube[5].z);
  glVertex3f(vertexCube[4].x,vertexCube[4].y,vertexCube[4].z);
  glEnd();

    glBegin(GL_POLYGON);     
  glVertex3f(vertexCube[5].x,vertexCube[5].y,vertexCube[5].z);
  glVertex3f(vertexCube[1].x,vertexCube[1].y,vertexCube[1].z);
  glVertex3f(vertexCube[2].x,vertexCube[2].y,vertexCube[2].z);
  glVertex3f(vertexCube[6].x,vertexCube[6].y,vertexCube[6].z);
  glEnd();

    glBegin(GL_POLYGON);     
  glVertex3f(vertexCube[7].x,vertexCube[7].y,vertexCube[7].z);
  glVertex3f(vertexCube[6].x,vertexCube[6].y,vertexCube[6].z);
  glVertex3f(vertexCube[2].x,vertexCube[2].y,vertexCube[2].z);
  glVertex3f(vertexCube[3].x,vertexCube[3].y,vertexCube[3].z);
  glEnd();

   glBegin(GL_POLYGON);     
  glVertex3f(vertexCube[0].x,vertexCube[0].y,vertexCube[0].z);
  glVertex3f(vertexCube[4].x,vertexCube[4].y,vertexCube[4].z);
  glVertex3f(vertexCube[7].x,vertexCube[7].y,vertexCube[7].z);
  glVertex3f(vertexCube[3].x,vertexCube[3].y,vertexCube[3].z);
  glEnd();

    glBegin(GL_POLYGON);     
  glVertex3f(vertexCube[4].x,vertexCube[4].y,vertexCube[4].z);
  glVertex3f(vertexCube[5].x,vertexCube[5].y,vertexCube[5].z);
  glVertex3f(vertexCube[6].x,vertexCube[6].y,vertexCube[6].z);
  glVertex3f(vertexCube[7].x,vertexCube[7].y,vertexCube[7].z);
  glEnd();

  glPopMatrix();
}

/*
void stateSync(RBState& rbState, RBStateDot& rbStateDot){
  rbStateDot.v = rbState.P/mass;
  
  Matrix3x3 R = rbState.q.rotation();
  Matrix3x3 Iinv = R*I0.inv()*(R.transpose());
  Vector3d w = Iinv*rbState.L;
  rbStateDot.qD = w*rbState.q/2;

  rbStateDot.F = Vector3d(0,0,0);
  rbStateDot.F = rbStateDot.F + mass*G;
  for(int i = 0; i<8; i++){
    rbStateDot.F = rbStateDot.F + vertexForce[i];
  }

  rbStateDot.T = Vector3d(0,0,0);
  for(int i = 0; i < 8; i++){
    rbStateDot.T = rbStateDot.T + (vertexCube[i] - rbState.x)%vertexForce[i];
  }
}
*/
RBStateDot stateSync(RBState& rbState){
  RBStateDot rbStateDot;
  rbStateDot.v = rbState.P/mass;
  
  Matrix3x3 R = rbState.q.rotation();
  //cout << "R "<<R<<endl;
  Matrix3x3 Iinv = R*I0.inv()*(R.transpose());
  Vector3d w = Iinv*rbState.L;
  rbStateDot.qD = w*rbState.q*0.5;   //notice the difference between * and /;

  rbStateDot.F = Vector3d(0,0,0);
  rbStateDot.F = rbStateDot.F + mass*G;
  for(int i = 0; i<8; i++){
    rbStateDot.F = rbStateDot.F + vertexForce[i];
  }

  rbStateDot.T = Vector3d(0,0,0);
  for(int i = 0; i < 8; i++){
    rbStateDot.T = rbStateDot.T + (vertexCube[i] - rbState.x)%vertexForce[i];
  }

  return rbStateDot;
}

void stateInt(RBState& rbState, RBStateDot& rbStateDot){        //using RK4
  RBStateDot k1 = stateSync(rbState);

  // cout << k1.v<< endl;
  RBState t1 = rbState + k1*TimeStep*0.5;

  RBStateDot k2 = stateSync(t1);
  RBState t2 = rbState + k2*TimeStep*0.5;

  RBStateDot k3 = stateSync(t2);
  RBState t3 = rbState + k3*TimeStep;

  RBStateDot k4 = stateSync(t3);
  //cout << k4.v << endl;

  
  rbStateDot = (k1 + k2*2 + k3*2 + k4)*(1.0/6.0);  // cost so much time
  rbState = rbState + rbStateDot*TimeStep;
  //cout << rbStateDot.v << endl;
  //cout<< "x"<<rbState.x <<endl;

  Matrix3x3 R = rbState.q.rotation();

  vertexCube[0] = R*Vector3d(lengthEdge/2,lengthEdge/2,lengthEdge/2) + rbState.x;
  vertexCube[1] = R*Vector3d(lengthEdge/2,lengthEdge/2,-lengthEdge/2) + rbState.x;
  vertexCube[2] = R*Vector3d(-lengthEdge/2,lengthEdge/2,-lengthEdge/2) + rbState.x;
  vertexCube[3] = R*Vector3d(-lengthEdge/2,lengthEdge/2,lengthEdge/2) + rbState.x;
  vertexCube[4] = R*Vector3d(lengthEdge/2,-lengthEdge/2,lengthEdge/2) + rbState.x;
  vertexCube[5] = R*Vector3d(lengthEdge/2,-lengthEdge/2,-lengthEdge/2) + rbState.x;
  vertexCube[6] = R*Vector3d(-lengthEdge/2,-lengthEdge/2,-lengthEdge/2) + rbState.x;
  vertexCube[7] = R*Vector3d(-lengthEdge/2,-lengthEdge/2,lengthEdge/2) + rbState.x;
     
}


void simulate(int){
  Vector3d ballPosition = Ball + BallVelocity*TimeStep;
  Vector3d newBallV = BallVelocity + G*TimeStep;
  if(ballPosition.x + BallRadius >= -lengthEdge/2){
    cubeMoving = true;
    changeColor = true;
    newBallV.x = -newBallV.x*0.4;
    newBallV.y = newBallV.y*0.8;
    newBallV.z = newBallV.z*0.8;
    ballPosition.x = ballPosition.x - errorTh;

    rbState.P = Vector3d(0.4,0.2,0.1);
    rbState.L = Vector3d(-0.3,0.15,1.5);    
  }

  Ball = ballPosition;
  BallVelocity = newBallV;
  
  if(cubeMoving){
    stateInt(rbState, rbStateDot);
    // cout<< "vertex 0 "<< vertexCube[0]<<endl;
    
 
    for(int i = 0; i < 8; i++){
      if(vertexCube[i].y <= 4){
        rbState.P.y = -rbState.P.y;
        rbState.P.x = 0.8*rbState.P.x;
        rbState.P.z = 0.8*rbState.P.z;
      }
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
  glClearColor(0.2, 0.2, 0.2, 0.0);
  glShadeModel(GL_SMOOTH);
  glDepthRange(0.0, 1.0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);

  do_lights();

  //initialize state of rigid body
  rbState.x = bodyCenter;

  Vector3d w(0,0,0);
  rbStateDot.qD = 0.5*w*rbState.q;

  vertexCube[0] = Vector3d(lengthEdge/2,lengthEdge/2,lengthEdge/2) + rbState.x;
  vertexCube[1] = Vector3d(lengthEdge/2,lengthEdge/2,-lengthEdge/2) + rbState.x;
  vertexCube[2] = Vector3d(-lengthEdge/2,lengthEdge/2,-lengthEdge/2) + rbState.x;
  vertexCube[3] = Vector3d(-lengthEdge/2,lengthEdge/2,lengthEdge/2) + rbState.x;
  vertexCube[4] = Vector3d(lengthEdge/2,-lengthEdge/2,lengthEdge/2) + rbState.x;
  vertexCube[5] = Vector3d(lengthEdge/2,-lengthEdge/2,-lengthEdge/2) + rbState.x;
  vertexCube[6] = Vector3d(-lengthEdge/2,-lengthEdge/2,-lengthEdge/2) + rbState.x;
  vertexCube[7] = Vector3d(-lengthEdge/2,-lengthEdge/2,lengthEdge/2) + rbState.x;

  for(int i =0; i<8; i++){
    vertexForce[i] = Vector3d(0,0,0);
  }

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
    makeGrid();
  }

  drawBall();

  drawCube();
  
  
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
  // initSimulation(argc, argv);

  // set up opengl callback functions
  glutDisplayFunc(PerspDisplay);
  glutMouseFunc(mouseEventHandler);
  glutMotionFunc(motionEventHandler);
  glutKeyboardFunc(keyboardEventHandler);

  glutMainLoop();
  return(0);
}

