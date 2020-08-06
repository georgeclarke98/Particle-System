////////////////////////////////////////////////////////////////
// Department of Computer Science
// The University of Manchester
//
// This code is licensed under the terms of the Creative Commons
// Attribution 2.0 Generic (CC BY 3.0) License.
//
// Skeleton code for COMP37111 coursework, 2019-20
//
// Authors: Toby Howard and Arturs Bekasovs
//
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#ifdef MACOSX
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cglm/cglm.h>

#define TOP_VIEW 1
#define SIDE_VIEW 2
#define FLY_VIEW 3
#define BEST_VIEW 4
#define DEG_TO_RAD 0.01745329
#define PI 3.141593
#define FLY_SPEED  2
#define TURN_ANGLE 4.0

// Display list for coordinate axis
GLuint axisList;

int AXIS_SIZE= 20;
int axisEnabled= 1;
int current_view, gravitySet, velocitySet, frames;
float date, current_time, last_time, time_diff, gravityCurrent, fpsTime, velocityNow;

GLint width= 1400, height= 1000;

GLdouble lat,     lon;              /* View angles (degrees)    */
GLdouble mlat,    mlon;             /* Mouse look offset angles */
GLfloat  eyex,    eyey,    eyez;    /* Eye point                */
GLfloat  centerx, centery, centerz; /* Look point               */
GLfloat  upx,     upy,     upz;
//variables for moving

///////////////////////////////////////////////

struct Particle {
  vec3 pos, velocity;
  vec4 colour;
  float life, gravity;
  //particle structure
  int time, reached;
};

const int maxParticles = 100000;
int particle_rate;
struct Particle particleContainer[maxParticles];
int lastUsedParticle = 0;
//create a container to hold all of the particles with size maximum of 100000

///////////////////////////////////////////////

int findFreeParticle() {
  int i;
  for(i = lastUsedParticle; i < maxParticles; i++) {
    if(particleContainer[i].life < 0) {
      lastUsedParticle = i;
      return i;
    } //if
  } //for

  for(i = 0; i < lastUsedParticle; i++) {
    if(particleContainer[i].life < 0) {
      lastUsedParticle = i;
      return i;
    } //if
  } //for

  return 0;
  //override the first particle
} //findFreeParticle

///////////////////////////////////////////////

void initContainer(int noParticles) {
  int i;
  for (i = 0; i < noParticles; i++) {
    particleContainer[i].life = -1.0;
  } //for
} //initContainer

///////////////////////////////////////////////

/* Given an eyepoint and latitude and longitude angles, will
     compute a look point one unit away */
void calculate_lookpoint(void) {

  GLfloat dirx, diry, dirz;

  if ((lat + mlat) <= 90 && (lat + mlat) >= -90) {
    dirx = cos((lat + mlat) * DEG_TO_RAD) * sin((lon + mlon) * DEG_TO_RAD) * 1000;
    diry = sin((lat + mlat) * DEG_TO_RAD) * 1000;
    dirz = cos((lat + mlat) * DEG_TO_RAD) * cos((lon + mlon) * DEG_TO_RAD) * 1000;
    //calculate directions

    centerx = eyex + dirx;
    centery = eyey + diry;
    centerz = eyez + dirz;
    //calculate centres from eye and direction
  }
} // calculate_lookpoint()

///////////////////////////////////////////////

void setView (void) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  switch (current_view) {
  case TOP_VIEW:
    gluLookAt(0.0, 50.0, 0.0,  // camera position
            0.0, 0.0, 0.0,    // point to look at
            0.0, 0.0, 1.0 );  // "upright" vector
            //look at the centre from up on the y-axis
    break;
  case SIDE_VIEW:
  gluLookAt(0.0, 0.0, 50.0,  // camera position
            0.0, 0.0, 0.0,    // point to look at
            0.0, 1.0, 0.0 );  // "upright" vector
            //look at the centre from along the z-axis to see orbits
    break;
    case FLY_VIEW:
    calculate_lookpoint(); /* Compute the centre of interest   */
    gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
      break;
  case BEST_VIEW:
  gluLookAt(40.0, 20.0, 40.0,  // camera position
            0.0, 0.0, 0.0,    // point to look at
            0.0, 1.0, 0.0 );  // "upright" vector
            //look at the centre from along the z-axis to see orbits
    break;
  }
}

///////////////////////////////////////////////

void menu (int menuentry) {
  switch (menuentry) {
  case 1: current_view= TOP_VIEW;
          break;
  case 2: current_view= SIDE_VIEW;
          break;
  case 3: current_view= FLY_VIEW;
          break;
  case 4: current_view= BEST_VIEW;
          break;
  case 5: exit(0);
  }
} //menu

///////////////////////////////////////////////

//Return random double within range [0,1]
double myRandom()
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  setView();

  // If enabled, draw coordinate axis
  if(axisEnabled) glCallList(axisList);

  current_time = glutGet(GLUT_ELAPSED_TIME);
  time_diff = (current_time - last_time) / 1000;

  last_time = current_time;

  // frames++;
  // if ((current_time / 1000) - fpsTime >= 1.0) {
  //   printf("%d fps %d\n", frames, particle_rate);
  //   frames = 0;
  //   fpsTime += 1.0;
  // } //if

  int new_particles = particle_rate;

  int i;
  for (i = 0; i < new_particles; i++) {
    int index = findFreeParticle();
    particleContainer[index].life = 5.0f;
    particleContainer[index].gravity = gravityCurrent;
    particleContainer[index].time = current_time;
    particleContainer[index].reached = 0;
    particleContainer[index].pos[0] = 0.0f;
    particleContainer[index].pos[1] = 0.0f;
    particleContainer[index].pos[2] = 0.0f;
    //initialise the next free particles life and position

    float a = myRandom() * 2 * PI;
    float r = 5 * sqrt(myRandom());
    //calculations for the fountain being in a circle

    particleContainer[index].velocity[0] = (float) r * cos(a);
    particleContainer[index].velocity[1] = velocityNow;
    particleContainer[index].velocity[2] = (float) r * sin(a);
    //apply the velocity to the current particle, random x and z and constant y

    particleContainer[index].colour[0] =  (double) myRandom();
    particleContainer[index].colour[1] =  (double) myRandom();
    particleContainer[index].colour[2] =  (double) myRandom();
    particleContainer[index].colour[3] = (int) ((double) myRandom() / 3);
    //randomise the colour of the particles
  } //for

  glPushMatrix();
  glBegin(GL_LINES);
  int particleCount = 0;
  for(i = 0; i < maxParticles; i++) {

    if(particleContainer[i].life > 0.0f) {
      particleContainer[i].life -= time_diff;

      if(particleContainer[i].life > 0.0f) {
        particleContainer[i].velocity[1] -= (float) particleContainer[i].gravity * time_diff;
        //update velocity withe the time difference between frames
        //only the y vector needs updating as only gravity is involved

        float oldX = particleContainer[i].pos[0];
        float oldY = particleContainer[i].pos[1];
        float oldZ = particleContainer[i].pos[2];

        particleContainer[i].pos[0] += (float) particleContainer[i].velocity[0] * time_diff;
        particleContainer[i].pos[1] += (float) particleContainer[i].velocity[1] * time_diff;
        particleContainer[i].pos[2] += (float) particleContainer[i].velocity[2] * time_diff;
        //update the position of the particle

        if (particleContainer[i].pos[1] <= 0)
          particleContainer[i].velocity[1] *= -1;

        // if((i % 100 == 0) && (particleContainer[i].pos[1] <= 0.0) && (particleContainer[i].reached == 0)) {
        //   float bottomTime = (float) (current_time - particleContainer[i].time) / 1000;
        //   printf("%f \n", bottomTime);
        //   particleContainer[i].reached = 1;
        // }

        glColor3f((double)particleContainer[i].colour[0],
                  (double)particleContainer[i].colour[1],
                  (double)particleContainer[i].colour[2]);
        glVertex3f(oldX, oldY, oldZ);
        glVertex3f(particleContainer[i].pos[0],
                   particleContainer[i].pos[1],
                   particleContainer[i].pos[2]);


      } //if
      else {
        particleContainer[i].pos[0] = 0.0f;
        particleContainer[i].pos[1] = 0.0f;
        particleContainer[i].pos[2] = 0.0f;
        glColor3f(particleContainer[i].colour[0], particleContainer[i].colour[1], particleContainer[i].colour[2]);
        glVertex3f(particleContainer[i].pos[0], particleContainer[i].pos[1], particleContainer[i].pos[2]);
      }

      particleCount++;
    } //if
  } //for
  glEnd();
  glPopMatrix();

  glutSwapBuffers();
  glutPostRedisplay();
}

///////////////////////////////////////////////

void reshape(int width, int height)
{
  glClearColor(0.9, 0.9, 0.9, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
}

///////////////////////////////////////////////

void makeAxes() {
// Create a display list for drawing coord axis
  axisList = glGenLists(1);
  glNewList(axisList, GL_COMPILE);
      glLineWidth(2.0);
      glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, AXIS_SIZE);
    glEnd();
  glEndList();
}

///////////////////////////////////////////////

void mouse_motion(int x, int y) {
  if(current_view == FLY_VIEW) {
    if (x >= 0 && x <= width && y >= 0 && y <= height) {
      mlon = 50 - (100 * ((double) x / width));
      mlat = 50 - (100 * ((double) y / height));
      //calculate mlon and mlat
      //lon = lon + mlon;
      //if (-90 <= lat + mlat <= 90)
      //  lat = lat + mlat;
    }
  }
} // mouse_motion()

///////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:  /* Escape key */
      exit(0);
      break;
    if(current_view == FLY_VIEW) {
      case 44 : //comma
        eyex = eyex + (sin(((lon + mlon) + 90) *  DEG_TO_RAD) * FLY_SPEED);
        eyez = eyez + (cos(((lon + mlon) + 90) * DEG_TO_RAD) * FLY_SPEED);
        break;
      case 46 : //full stop
        eyex = eyex + (sin(((lon + mlon) - 90) *  DEG_TO_RAD) * FLY_SPEED);
        eyez = eyez + (cos(((lon + mlon) - 90) * DEG_TO_RAD) * FLY_SPEED);
        break;
      case 112 : //p
        switch (particle_rate) {
          case 4000 :
            particle_rate = 2;
            break;
          case 2 : //1
            particle_rate = 10;
            break;
          case 10 : //2
            particle_rate = 20;
            break;
          case 20 : //3
            particle_rate = 50;
            break;
          case 50 : //4
            particle_rate = 100;
            break;
          case 100 : //5
            particle_rate = 250;
            break;
          case 250 : //5
            particle_rate = 500;
            break;
          case 500 : //6
            particle_rate = 750;
            break;
          case 750 :
            particle_rate = 1000;
            break;
          case 1000 : //7
            particle_rate = 1500;
            break;
          case 1500 : //7
            particle_rate = 2000;
            break;
          case 2000 : //8
            particle_rate = 2500;
            break;
          case 2500 : //8
            particle_rate = 3000;
            break;
          case 3000 : //8
            particle_rate = 3500;
            break;
          case 3500 : //8
            particle_rate = 4000;
            break;
        }
        break;
      case 103 : //g
        switch (gravitySet) {
          case 1:
            gravityCurrent = 19.62;
            gravitySet = 2;
          break;
          case 2:
            gravityCurrent = 39.24;
            gravitySet = 3;
          break;
          case 3:
            gravityCurrent = 9.81;
            gravitySet = 1;
          break;
        }
        break;
      case 118 : //void
        switch(velocitySet) {
          case 0:
            velocityNow = 25.0;
            velocitySet = 1;
            break;
          case 1:
            velocityNow = 30.0;
            velocitySet = 2;
            break;
          case 2:
            velocityNow = 10.0;
            velocitySet = 3;
            break;
          case 3:
            velocityNow = 15.0;
            velocitySet = 4;
            break;
          case 4:
            velocityNow = 20.0;
            velocitySet = 0;
            break;
        } //switch
        break;
    }
  }
} // keyboard()

//////////////////////////////////////////////

void cursor_keys(int key, int x, int y) {
  if(current_view == FLY_VIEW) {
    switch (key) {
      case GLUT_KEY_LEFT :
        lon = lon + TURN_ANGLE;
        break;
      case GLUT_KEY_RIGHT :
        lon = lon - TURN_ANGLE;
        break;
      case GLUT_KEY_PAGE_UP :
        lat = lat + TURN_ANGLE;
        break;
      case GLUT_KEY_PAGE_DOWN :
        lat = lat - TURN_ANGLE;
        break;
      case GLUT_KEY_HOME :
      eyex = 300000000.0;
      eyey = 150000000.0;
      eyez = 300000000.0;
      upx = 0.0;
      upy = 1.0;
      upz = 0.0;
      lat = -30.0;
      lon = 220.0;
      mlat = 0.0;
      mlon = 0.0;

        break;
      case GLUT_KEY_UP :
        eyex = eyex + (sin((lon + mlon) *  DEG_TO_RAD) * FLY_SPEED);
        eyey = eyey + (sin((lat + mlat) *  DEG_TO_RAD) * FLY_SPEED);
        eyez = eyez + (cos((lon + mlon) * DEG_TO_RAD) * FLY_SPEED);
        break;
      case GLUT_KEY_DOWN :
        eyex = eyex - (sin((lon + mlon) *  DEG_TO_RAD) * FLY_SPEED);
        eyey = eyey - (sin((lat + mlat) *  DEG_TO_RAD) * FLY_SPEED);
        eyez = eyez - (cos((lon + mlon) * DEG_TO_RAD) * FLY_SPEED);
        break;
    } //switch
    //set cursor keys to change lat and lon respectively and to move
  }
} // cursor_keys()

//////////////////////////////////////////////

void initGraphics(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("COMP37111 Particles");

  /* Set initial view parameters */
  /* Define background colour */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  eyex = 40.0;
  eyey = 20.0;
  eyez = 40.0;
  upx = 0.0;
  upy = 1.0;
  upz = 0.0;
  lat = -40.0;
  lon = 200.0;
  mlat = 0.0;
  mlon = 0.0;

  glutCreateMenu (menu);
  glutAddMenuEntry ("Top view", 1);
  glutAddMenuEntry ("Side view", 2);
  glutAddMenuEntry ("Fly view", 3);
  glutAddMenuEntry ("Best view", 4);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Quit", 4);
  glutAttachMenu (GLUT_RIGHT_BUTTON);

  current_view= BEST_VIEW;

  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutSpecialFunc (cursor_keys);
  glutPassiveMotionFunc (mouse_motion);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  makeAxes();
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  current_time = 0.0;
  last_time = 0.0;
  fpsTime = 0.0;
  particle_rate = 2;
  gravityCurrent = 9.81;
  gravitySet = 1;
  frames = 0;
  velocityNow = 20.0;
  velocitySet = 0;
  initContainer(maxParticles);
  glutMainLoop();
}
