/**************************************************
  InteractingParticles.cpp
  Student Name: Yang Guo
  Assignment#03  CPSC8170  
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


int WIDTH = 1000;
int HEIGHT = 800;

int persp_win;

Camera *camera;

bool generate = true;
bool ballParticle = false;
int  fly = 0;
bool showGrid = true;

// Declaration of important global variables
static float Box[3] = {16, 16, 16};          // length,width,height of box are all 20
static Vector3d Ball(0, 8, 0);   // initial position of the ball
static float BallRadius = 0.4;

static Vector3d VelocityOrig(10, 0, 0);
static Vector3d Velocity(0, 0, 0);
static Vector3d G(0, -9.8, 0);
static double Mass = 50;
static double Viscosity = 10;
static double CRestitution = 1;
static double TimeStep = 0.08;
static int TimerDelay = 25;  // yaogai

// particle parameter
static float ParticleRadius = 0.4;

// 0,1 2,3,4,5 elements of the array represent front, back, left, right, upper, bottom sides of the box.
static Vector3d PointsPlane[6];
static Vector3d NormsPlane[6];

static Vector3d newBall;

// particlelist
std::vector<Particle> ParticleList;
const int pNum = 100;

Vector3d particleStateVector[pNum*2];
Vector3d newParticleStateVector[pNum*2];


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
  
  if(fscanf(paramfile, "%lf %lf %lf %lf %lf %lf %lf %f",
	    &Mass, &(VelocityOrig.x), &(VelocityOrig.y), &(VelocityOrig.z), &Viscosity, &CRestitution,
	    &TimeStep, &ParticleRadius) != 8){
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


  Velocity = VelocityOrig;
  Ball = Vector3d(0, Box[2]/2, 0);

}

void drawBox(){
  glColor3f(0.882, 0.878, 0.796);

  glLineWidth(2.0);

  glBegin(GL_LINES);
  glVertex3f(-Box[0]/2,0,-Box[1]/2);
  glVertex3f(-Box[0]/2,Box[2],-Box[1]/2);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(-Box[0]/2,0,Box[1]/2);
  glVertex3f(-Box[0]/2,Box[2],Box[1]/2);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(Box[0]/2,0,-Box[1]/2);
  glVertex3f(Box[0]/2,Box[2],-Box[1]/2);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(Box[0]/2,0,Box[1]/2);
  glVertex3f(Box[0]/2,Box[2],Box[1]/2);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(-Box[0]/2,Box[2],-Box[1]/2);
  glVertex3f(Box[0]/2,Box[2],-Box[1]/2);
  glEnd();
  
  glBegin(GL_LINES);
  glVertex3f(-Box[0]/2,Box[2],-Box[1]/2);
  glVertex3f(-Box[0]/2,Box[2],Box[1]/2);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(Box[0]/2,Box[2],-Box[1]/2);
  glVertex3f(Box[0]/2,Box[2],Box[1]/2);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(-Box[0]/2,Box[2],Box[1]/2);
  glVertex3f(Box[0]/2,Box[2],Box[1]/2);
  glEnd();
  
  glBegin(GL_LINES);
  glVertex3f(-Box[0]/2,0,-Box[1]/2);
  glVertex3f(Box[0]/2,0,-Box[1]/2);
  glEnd();
  
  glBegin(GL_LINES);
  glVertex3f(-Box[0]/2,0,-Box[1]/2);
  glVertex3f(-Box[0]/2,0,Box[1]/2);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(Box[0]/2,0,-Box[1]/2);
  glVertex3f(Box[0]/2,0,Box[1]/2);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(-Box[0]/2,0,Box[1]/2);
  glVertex3f(Box[0]/2,0,Box[1]/2);
  glEnd();
      
  /*
  glBegin(GL_POLYGON);       
  glColor3f(0.0, 0.0, 0.0);
  glVertex3f(-Box[0]/2, 0, Box[1]/2);
  glVertex3f(Box[0]/2, 0, Box[1]/2);
  glVertex3f(Box[0]/2, 0, -Box[1]/2); 
  glVertex3f(-Box[0]/2, 0, -Box[1]/2);
  glEnd();
  
  glBegin(GL_POLYGON);       
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(-Box[0]/2, 0, -Box[1]/2);
  glVertex3f(Box[0]/2, 0, -Box[1]/2);
  glVertex3f(Box[0]/2, Box[2], -Box[1]/2); 
  glVertex3f(-Box[0]/2, Box[2], -Box[1]/2);
  glEnd();

  glBegin(GL_POLYGON);       
  glColor3f(0.4, 0.4, 0.0);
  glVertex3f(-Box[0]/2, 0, -Box[1]/2);
  glVertex3f(-Box[0]/2, 0, Box[1]/2);
  glVertex3f(-Box[0]/2, Box[2], Box[1]/2); 
  glVertex3f(-Box[0]/2, Box[2], -Box[1]/2);
  glEnd();
  
  glBegin(GL_POLYGON);       
  glColor3f(1.0, 0.0, 0.5);
  glVertex3f(Box[0]/2, 0, -Box[1]/2);
  glVertex3f(Box[0]/2, 0, Box[1]/2);
  glVertex3f(Box[0]/2, Box[2], Box[1]/2); 
  glVertex3f(Box[0]/2, Box[2], -Box[1]/2);
  glEnd();
  
  glBegin(GL_POLYGON);       
  glColor3f(0.7, 0.7, 0.7);
  glVertex3f(Box[0]/2, Box[2], -Box[1]/2);
  glVertex3f(Box[0]/2, Box[2], Box[1]/2);
  glVertex3f(-Box[0]/2, Box[2], Box[1]/2); 
  glVertex3f(-Box[0]/2, Box[2], -Box[1]/2);
  glEnd();
  */
  
}

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

int checkResting(){
  int num = -1;
  for(int i =0; i<6; i++){
    if( Abs((Ball - PointsPlane[i]) * NormsPlane[i] - BallRadius) < EPS &&
	Abs( Velocity * NormsPlane[i]) < EPS ){
      num = i;
    }
  }
  return num+1;
}

bool checkCollision(){
  if(newBall[0] - BallRadius< -Box[0]/2 || newBall[0] + BallRadius> Box[0]/2 ||
     newBall[1] - BallRadius< 0 || newBall[1] + BallRadius > Box[2] ||
     newBall[2] - BallRadius< -Box[1]/2 || newBall[2] + BallRadius> Box[1]/2)
    return true;
  else
    return false;
}


Vector3d Accel(){
  Vector3d accel = G - Viscosity * Velocity / Mass;
  return accel;
  
}

void particleGenerator(){
  for(int i = 0; i < pNum; i++){
    ParticleList.push_back(Particle(Vector3d(gauss(0,4,1), gauss(8,4,1), gauss(0,4,1)), Vector3d(0,0,0), Vector4d(0.996,0.588,0,0), Vector3d(0,0,0), 1, 1, false));
  }
}

Vector3d accelStateVector[pNum*2];

void simulate(int){
  
  /*
  if(generate){
    particleGenerator();
    
    for(int i = 0; i < pNum; i++){
      std::cout << ParticleList[i].position << std::endl;
    }
    
    generate = false;
  }
  */
  
  Vector3d acceleration;
  Vector3d newVelocity;
  bool resting;

  acceleration = Accel();

  resting = false;
  int resNum;
  if( resNum = checkResting()){
    resting = true;

    if( acceleration * NormsPlane[resNum-1] > 0){
      resting = false;
    }
    else{
      //std::cout << resNum << endl;
      acceleration = acceleration - (acceleration * NormsPlane[resNum -1]) * NormsPlane[resNum-1];
      Velocity = Velocity - (Velocity * NormsPlane[resNum -1] * NormsPlane[resNum -1]);
      Ball =  Ball - ((Ball - PointsPlane[resNum -1]) * NormsPlane[resNum -1] - BallRadius) *NormsPlane[resNum -1];
    }
  }

  newVelocity = Velocity + TimeStep * acceleration;
  newBall = Ball + TimeStep * Velocity;


  while(checkCollision()){
    int i;
    float rat = 1.0;
    float ratio[6] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    int cNum = -1;	
    for(i =0; i < 6; i++){
      if((Ball - PointsPlane[i]) * NormsPlane[i] - BallRadius < (Ball - newBall) * NormsPlane[i]){
	ratio[i] = ( (Ball - PointsPlane[i]) * NormsPlane[i] - BallRadius) / ( (Ball - newBall) * NormsPlane[i] );
	//std::cout << i << endl;
	//std::cout << ratio[i] << endl;
      }
      if( ratio[i] < rat){
	rat  = ratio[i];
	cNum = i;
      }
    }	
    
    Vector3d velocityC, positionC, v1;
    
    velocityC = Velocity + rat * TimeStep * acceleration; 
    positionC = Ball + rat * TimeStep * Velocity;   	

    v1 = ( velocityC *  NormsPlane[cNum] ) * NormsPlane[cNum] *(-CRestitution )+ (velocityC - ( velocityC *  NormsPlane[cNum] ) * NormsPlane[cNum]);
    Velocity = v1;
    Ball = positionC;
    
    double CollisionTimeStep;
    acceleration = Accel();
    CollisionTimeStep = (1-rat) * TimeStep;
    newVelocity = Velocity + CollisionTimeStep * acceleration; 
    newBall = Ball + CollisionTimeStep * Velocity;

    //std::cout << newBall <<endl;
  }
 
  Ball = newBall;
  Velocity = newVelocity;
  
  //std::cout << "origin" << Ball << endl;

  // drawBall();

  // flock simulation
  for(int i = 0; i < pNum; i++){
    accelStateVector[pNum+i] = Vector3d(0,0,0);
  }

  double Ka = 2;
  double Kv = 1;
  double Kc = 0.3;

  double K1 = 4;
  double K2 = 4;
  double K3 = 1;
  
  for(int i = 0; i < pNum; i++){
    // calculate the interaction between particles and the Ball
    Vector3d vPtoBall = Ball - particleStateVector[i];
    double lPtoBall = vPtoBall.norm();

    if(lPtoBall <= (ParticleRadius + BallRadius + 0.1)){
      particleStateVector[pNum+i] = particleStateVector[pNum+i] -2*(particleStateVector[pNum+i]*vPtoBall.normalize()*vPtoBall.normalize());
    }

    // collision avoidance
    Vector3d accelParticleBallAvoid = -1*K1*vPtoBall.normalize()/lPtoBall;

    // velocity matching
    Vector3d accelParticleBallMatch = K2*(Velocity - particleStateVector[pNum+i]);
    
    // centering
    Vector3d accelParticleBallCenter = K3*vPtoBall;

    accelStateVector[pNum+i] = accelParticleBallAvoid + accelParticleBallMatch + accelParticleBallCenter;

    for(int j = 0; j < pNum; j++){
      if(j != i){
        Vector3d vPtoP = particleStateVector[j] - particleStateVector[i];
        double lPtoP = vPtoP.norm();

        if(lPtoP <= ParticleRadius*5/2 + 0.02){
          particleStateVector[pNum+i] = particleStateVector[pNum+i] -2*(particleStateVector[pNum+i]*vPtoP.normalize()*vPtoP.normalize());
        }

        Vector3d accelPtoPAvoid = -1*Ka*vPtoP.normalize()/lPtoP;
        Vector3d accelPtoPMatch = Kv*(particleStateVector[pNum+j] - particleStateVector[pNum+i]);
        Vector3d accelPtoPCenter = Kc*vPtoP;

        accelStateVector[pNum+i] = accelStateVector[pNum+i] + accelPtoPAvoid + accelPtoPMatch + accelPtoPCenter;
      }
    }

  }

  for(int i = 0; i < pNum; i++){
    particleStateVector[pNum+i] =  particleStateVector[pNum+i] + accelStateVector[pNum+i]*TimeStep;
    particleStateVector[i] = particleStateVector[i] + particleStateVector[pNum+i]*TimeStep;

    ParticleList[i].position = particleStateVector[i];
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
  glClearColor(0.54, 0.54, 0.54, 0.0);
  glShadeModel(GL_SMOOTH);
  glDepthRange(0.0, 1.0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);

  // init value of Points and Norms in each plane, points are chosen to be center of each plane.
  PointsPlane[0] = Vector3d(0, 0, Box[1]/2);
  NormsPlane[0]  = Vector3d(0, 0, -1);

  PointsPlane[1] = Vector3d(0, 0, -Box[1]/2);
  NormsPlane[1]  = Vector3d(0, 0, 1);

  PointsPlane[2] = Vector3d(-Box[0]/2, 0, 0);
  NormsPlane[2]  = Vector3d(1, 0, 0);

  PointsPlane[3] = Vector3d(Box[0]/2, 0, 0);
  NormsPlane[3]  = Vector3d(-1, 0, 0);

  PointsPlane[4] = Vector3d(0, Box[2], 0);
  NormsPlane[4]  = Vector3d(0, -1, 0);

  PointsPlane[5] = Vector3d(0, 0, 0);
  NormsPlane[5]  = Vector3d(0, 1, 0);

  particleGenerator();
  
  for(int i = 0; i < pNum; i++){
    particleStateVector[i] = ParticleList[i].position;
    particleStateVector[pNum+i] = ParticleList[i].velocity;
    //accelStateVector[i] = ParticleList[i].velocity;
    //accelStateVector[pNum+i] = ParticleList[i].accel;
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
    glDisable(GL_LIGHTING);
    makeGrid();
  }
  drawBox();
  do_lights();
  
  drawBall();


  // draw particles
  // choose which particle do you want
  if(ballParticle){
    for(int i = 0; i < ParticleList.size(); i++){

      GLfloat ball_mat_ambient[]  = {0.996f, 0.588f, 0.07f, 1.0f};
      GLfloat ball_mat_diffuse[]  = {0.996f, 0.588f, 0.05f, 1.0f};
      GLfloat ball_mat_specular[] = {0.996f, 0.588f, 0.05f, 1.0f};
      GLfloat ball_mat_emission[] = {0.996f, 0.588f, 0.03f, 1.0f};
      GLfloat ball_mat_shininess  = 10.0f;
        
      glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
      glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
      glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
        
      glPushMatrix();
      // glColor4f(ParticleList[i].color.x, ParticleList[i].color.y, ParticleList[i].color.z, ParticleList[i].color.w);
      glTranslatef(ParticleList[i].position.x, ParticleList[i].position.y, ParticleList[i].position.z);
      glutSolidSphere(ParticleRadius, 20, 20);
      glPopMatrix();
    }
  }
  else{
    glDisable(GL_LIGHTING);
    for(int i = 0; i < ParticleList.size(); i++){
      Vector3d vP = particleStateVector[pNum+i].normalize();
      Vector3d vView = Vector3d(0,1,1);
      Vector3d vAxis = (vP%vView).normalize();
      Vector3d vFly = (vP%vAxis).normalize();
      glBegin(GL_POLYGON);
      glColor3f(0.735,0.657,0.743);
      glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).z);
      glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).z);
      glVertex3f((ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).z);
      glVertex3f((ParticleList[i].position+ParticleRadius/3*(-vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(-vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/-3*(vP+vAxis)).z);
      glEnd();

      glBegin(GL_POLYGON);
      glColor3f(0.996,0.588,0);
      glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).z);
      glVertex3f((ParticleList[i].position+ParticleRadius*(vP/2+vAxis/6)).x, (ParticleList[i].position+ParticleRadius*(vP/2+vAxis/6)).y, (ParticleList[i].position+ParticleRadius*(vP/2+vAxis/6)).z);
      glVertex3f((ParticleList[i].position+ParticleRadius*vP*11/30).x, (ParticleList[i].position+ParticleRadius*vP*11/30).y, (ParticleList[i].position+ParticleRadius*vP*11/30).z);
      glVertex3f((ParticleList[i].position+ParticleRadius*(vP/2-vAxis/6)).x, (ParticleList[i].position+ParticleRadius*(vP/2-vAxis/6)).y, (ParticleList[i].position+ParticleRadius*(vP/2-vAxis/6)).z);
      glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).z);
      glEnd();

      // move wings to fly
      if(fly%30 >=10 && fly%30 <=19){
        glBegin(GL_POLYGON);
        glColor3f(0.376,0.114,0.592);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(-vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(-vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/-3*(vP+vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius*vAxis*4/3).x, (ParticleList[i].position+ParticleRadius*vAxis*4/3).y, (ParticleList[i].position+ParticleRadius*vAxis*4/3).z);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(0.376,0.114,0.592);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/-3*(vP-vAxis)).z);
        glVertex3f((ParticleList[i].position-ParticleRadius*vAxis*4/3).x, (ParticleList[i].position-ParticleRadius*vAxis*4/3).y, (ParticleList[i].position-ParticleRadius*vAxis*4/3).z);
        glEnd();
      }
      else if(fly%30 >= 0 && fly%30 <= 9){
        glBegin(GL_POLYGON);
        glColor3f(0.376,0.114,0.592);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(-vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(-vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/-3*(vP+vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius*vAxis+ParticleRadius*vFly/3).x, (ParticleList[i].position+ParticleRadius*vAxis+ParticleRadius*vFly/3).y, (ParticleList[i].position+ParticleRadius*vAxis+ParticleRadius*vFly/3).z);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(0.376,0.114,0.592);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/-3*(vP-vAxis)).z);
        glVertex3f((ParticleList[i].position-ParticleRadius*vAxis+ParticleRadius*vFly/3).x, (ParticleList[i].position-ParticleRadius*vAxis+ParticleRadius*vFly/3).y, (ParticleList[i].position-ParticleRadius*vAxis+ParticleRadius*vFly/3).z);
        glEnd();
      }
      else if(fly%30 >= 20 && fly%30 <= 29){
        glBegin(GL_POLYGON);
        glColor3f(0.376,0.114,0.592);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP+vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(-vP+vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(-vP+vAxis)).y, (ParticleList[i].position+ParticleRadius/-3*(vP+vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius*vAxis-ParticleRadius*vFly/3).x, (ParticleList[i].position+ParticleRadius*vAxis-ParticleRadius*vFly/3).y, (ParticleList[i].position+ParticleRadius*vAxis-ParticleRadius*vFly/3).z);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(0.376,0.114,0.592);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/3*(vP-vAxis)).z);
        glVertex3f((ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).x, (ParticleList[i].position+ParticleRadius/3*(-vP-vAxis)).y, (ParticleList[i].position+ParticleRadius/-3*(vP-vAxis)).z);
        glVertex3f((ParticleList[i].position-ParticleRadius*vAxis-ParticleRadius*vFly/3).x, (ParticleList[i].position-ParticleRadius*vAxis-ParticleRadius*vFly/3).y, (ParticleList[i].position-ParticleRadius*vAxis-ParticleRadius*vFly/3).z);
        glEnd();
      }
      fly++;
      if(fly == 30000)
        fly = 0;
       

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
    Velocity = VelocityOrig;
    Ball = Vector3d(0, Box[2]/2, 0);
    glutTimerFunc(TimerDelay, simulate, 1);
    break;
  case 'p': case 'P':
    ballParticle = !ballParticle;
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
  persp_win = glutCreateWindow("Interacting Particles");

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

