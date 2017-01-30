/**************************************************
  Particles EveryWhere
  Student Name: Yang Guo
  Assignment02  CPSC8170  
 **************************************************/
//
// Example program to show how to use Chris Root's OpenGL Camera Class

// Christopher Root, 2006
// Minor Modifications by Donald House, 2009
// Minor Modifications by Yujie Shu, 2012

#include "Camera.h"
#include "gauss.h"
#include "Particle.h"
#include <cmath>
#include <vector>

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define EPS          0.1


int WIDTH = 640;
int HEIGHT = 480;

int persp_win;

Camera *camera;

bool showGrid = true;
bool showLight = false;

// Declaration of important global variables
vector<Particle> ParticleList;
vector<Particle>::iterator iteratorParticle;

static Vector3d Ball(0, 5, 0);   // initial position of the ball
static float BallRadius = 1;

static Vector3d Velocity(0, 0, 0);
static Vector3d G(0, 1, 0);
static Vector3d Wind(0,0,0);
static double DenseGenerator = 0.1;
static double CRestitution = 0.8;
static double TimeStep = 0.1;
static int TimerDelay = 25;  


// draws a simple grid
void makeGrid() {
  glColor3f(0.0, 0.0, 0.0);

  glLineWidth(1.0);

  for (float i=-12; i<12; i++) {
    for (float j=-12; j<12; j++) {
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
  glVertex3f(-12, 0, 0);
  glVertex3f(12, 0, 0);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(0, 0, -12);
  glVertex3f(0, 0, 12);
  glEnd();
  glLineWidth(1.0);
}

// particle generator

void particleGenerator(){
  for(double i = -1; i < 1; i += DenseGenerator)
    for(double j = -1; j < 1; j += DenseGenerator){
      // initialize particles
      Particle newParticle(Vector3d(i,0,j), Vector3d(gauss(0,1,1), gauss(2,0.6,1), gauss(0,0.8,1)), Vector4d(0.251, 0.643, 0.874, 1), 0, 1);
      ParticleList.push_back(newParticle);
    }
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
  
  if(fscanf(paramfile, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	    &(Ball.x), &(Ball.y), &(Ball.z), &(G.x), &(G.y), &(G.z), &(Wind.x),  &(Wind.y), &(Wind.z), &CRestitution,
	    &TimeStep) != 11){
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


void init() {
  // set up camera
  // parameters are eye point, aim point, up vector
  camera = new Camera(Vector3d(0, 0, 1), Vector3d(0, 0, 0), 
		      Vector3d(0, 1, 0));


  // grey background for window
  glClearColor(0.47, 0.47, 0.47, 0.0);
  glShadeModel(GL_SMOOTH);
  glDepthRange(0.0, 1.0);



  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);

  particleGenerator();
  
}



/*
 Initialize the Simulation
 */
void initSimulation(int argc, char* argv[]){

  if(argc != 2){
    fprintf(stderr, "usage: Particles Everywhere!\nYou should use parameter file!\n");
    exit(1);
  }
  
    LoadParameters(argv[1]);

}



Vector3d Accel(){
  Vector3d accel = G + Wind;
  return accel;
  
}

bool checkCollision(Vector3d particlePosition, Vector3d ballPosition){
  Vector3d lengthCollision = particlePosition - ballPosition;
  if(lengthCollision*lengthCollision < BallRadius*BallRadius)
    return true;
  else return false;
}

void simulate(int){
  particleGenerator();
  Vector3d acceleration;
  Vector3d newParticlePosition;
  Vector3d newParticleVelocity;
  acceleration = Accel();

  for(iteratorParticle = ParticleList.begin(); iteratorParticle != ParticleList.end(); iteratorParticle++){
    iteratorParticle->lifeSpan += 1;

    Vector3d accelOpt = acceleration;

    if(iteratorParticle->position.y > 10){
      accelOpt.y = -accelOpt.y;
    }

    newParticleVelocity = iteratorParticle->velocity + accelOpt*TimeStep;
    newParticlePosition = iteratorParticle->position +  iteratorParticle->velocity*TimeStep;

    if(checkCollision(newParticlePosition,Ball)){
      //when collision happens, change the color of particle and reset its position and velocity
      iteratorParticle->color = Vector4d(gauss(0.85,0.2,1), gauss(0.35,0.3,1), gauss(0.2,0.2,1), 0.6);
      Vector3d vectorOldToBall = Ball - iteratorParticle->position;
      Vector3d normOldToNew = (newParticlePosition - iteratorParticle->position).normalize();

      double l1 = vectorOldToBall * normOldToNew;
      double d1 = sqrt(vectorOldToBall.normsqr() - l1*l1);
      double l2 = sqrt(BallRadius*BallRadius - d1*d1);
      double ratioOldToCollision = (l1 - l2)/(newParticlePosition - iteratorParticle->position).norm();

      Vector3d collisionPoint = iteratorParticle->position + (newParticlePosition - iteratorParticle->position)*ratioOldToCollision; 
      Vector3d normBallToCollision = (collisionPoint - Ball).normalize();

      newParticlePosition = newParticlePosition  - (1 + CRestitution)*(newParticlePosition - collisionPoint)*normBallToCollision*normBallToCollision;
      newParticleVelocity = newParticleVelocity - (1 + CRestitution)*newParticleVelocity*normBallToCollision*normBallToCollision;

    
    }


    iteratorParticle->velocity = newParticleVelocity;
    iteratorParticle->position = newParticlePosition;

    
    if(iteratorParticle->lifeSpan >100){
      iteratorParticle = ParticleList.erase(ParticleList.begin(), ParticleList.begin() + 2000);
      --iteratorParticle;
    }
  }

    
  glutPostRedisplay();
  glutTimerFunc(TimerDelay, simulate, 1);
  
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

  for(int i = 0; i < ParticleList.size(); i++){
    glBegin(GL_POINTS);
    glPointSize(50);
    glColor4f(ParticleList[i].color.x, ParticleList[i].color.y, ParticleList[i].color.z, ParticleList[i].color.w);
    glVertex3f(ParticleList[i].position.x, ParticleList[i].position.y, ParticleList[i].position.z);
    glEnd();    
  }
  

  if (showGrid) 
    makeGrid();

  if(showLight)
    do_lights();
  
  glColor4f(0.85, 0.35, 0.10, 0.1);
  glTranslatef(Ball[0], Ball[1], Ball[2]);
  glutSolidSphere(BallRadius, 50, 50);
  
  
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
  case 'l': case'L':
    showLight = !showLight;
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
  persp_win = glutCreateWindow("Particles Everywhere");

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

