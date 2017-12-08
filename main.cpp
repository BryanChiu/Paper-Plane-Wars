#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include "Plane.h"
Plane *currentPlane;

#define X 0
#define Y 1
#define Z 2
#define W 3

float camPos[] = {0, 35, 10};	//where the camera is
float camTarget[] = {0, 34, 5};

int launchState = 0; // 0=none, 1=pitch, 2=yaw, 3=power
int wheelTimer;

void FloorMesh() {
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i=0; i>-100-1; i--) {
		glBegin(GL_QUAD_STRIP);
			for (int j=-50; j<50; j++) {
				glColor3f(0,0,0);
				glVertex3f(j, 0, i);
				glVertex3f(j, 0, i-1);
			}
		glEnd();
	}
}

void LaunchSequence() {
	wheelTimer++;
	if (wheelTimer>50) {
		wheelTimer = -50;
	}

	switch (launchState) {
		case 1:
			currentPlane->SetPitch(wheelTimer);
			break;

		case 2:
			currentPlane->SetYaw(wheelTimer);
			break;

		case 3:
			currentPlane->SetPower(wheelTimer);
			break;
	}

	// Show launch vector
	glLineWidth(5);
	glBegin(GL_LINES);
		glColor3f(0,0,0);
		glVertex3f(0,0,3);
		if (launchState==3) {
			glVertex3f(0,0,currentPlane->getPower()*-20 + 5);
		} else {
			glVertex3f(0,0,-16);
		}
	glEnd();
	glLineWidth(1);
}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], camTarget[0], camTarget[1], camTarget[2], 0,1,0);

	FloorMesh();

	glPushMatrix();
		std::vector<float> translateCoords = currentPlane->getCoords();
		glTranslatef(translateCoords[0], translateCoords[1], translateCoords[2]);

		std::vector<float> rotateAngles = currentPlane->getOrient();
		glRotatef(rotateAngles[0], 1, 0, 0); // x-axis rotation (pitch)
		glRotatef(rotateAngles[1], 0, 1, 0); // y-axis rotation (yaw)

		glPushMatrix();
			currentPlane->DrawPlane();
		glPopMatrix();

		// if (launchState == 0) {
		// 	glLineWidth(5);
		// 	glBegin(GL_LINES);
		// 		glColor3f(0,0,0);
		// 		glVertex3f(0,0,3);
		// 		glVertex3f(0,0,-3);
		// 	glEnd();
		// 	glLineWidth(1);
		// }
		if (launchState != 0) {
			LaunchSequence();
		}

		if (currentPlane->inFlight) {
			currentPlane->MovePlane();
		}

	glPopMatrix();

	//flush out to single buffer
	glutSwapBuffers();
}


//OpenGL functions
//mouse
void mouse(int btn, int state, int x, int y){
	int centerX = glutGet(GLUT_WINDOW_WIDTH)/2;
	int centerY = glutGet(GLUT_WINDOW_HEIGHT)/2;

	if (btn == GLUT_LEFT_BUTTON){
		if (state == GLUT_DOWN){
			switch (launchState) {
				case 0: // starts aiming process
					wheelTimer = 0;
					launchState = 1;
					break;

				case 1: // pitch set
					wheelTimer = 25;
					launchState = 2;
					break;

				case 2: // yaw set
					wheelTimer = -30;
					launchState = 3;
					break;

				case 3: // power set, launches
					wheelTimer = -30;
					launchState = 0;
					currentPlane->inFlight = true;
					currentPlane->LaunchPlane();
					break;
			}
		}
	}
}

void mouseMotion(int x, int y){
	// if (aiming) {
	// 	currentPlane->AimPlane(x, y);
	// }
}

void mousePassiveMotion(int x, int y){
	//printf("mousePassive coords: %i,%i\n", x, y);
}

//keyboard stuff
void keyboard(unsigned char key, int xIn, int yIn) {
	int mod = glutGetModifiers();
	switch (key) {
		case 'q':
		case 27:	//27 is the esc key
			exit(0);
			break;

		case 's':
			camTarget[Y] -= 0.1f;
			break;

		case 'w':
			camTarget[Y] += 0.1f;
			break;

		case 'a':
			camTarget[X] -= 0.1f;
			break;

		case 'd':
			camTarget[X] += 0.1f;
			break;

		case 'r':
			currentPlane = new Plane();
			currentPlane->InitPlane(-0.001, 1, 0, 32, 0, 0, 0);
			wheelTimer = -30;
			launchState = 0;
			break;
	}
}

void special(int key, int xIn, int yIn){
	switch (key){
		case GLUT_KEY_DOWN:
			camTarget[Y]-=0.2f;
			camPos[Y]-=0.2f;
			break;

		case GLUT_KEY_UP:
			camTarget[Y]+=0.2f;
			camPos[Y]+=0.2f;			
			break;

		case GLUT_KEY_LEFT:
			camTarget[X]-=0.2f;
			camPos[X]-=0.2f;
			break;

		case GLUT_KEY_RIGHT:
			camTarget[X]+=0.2f;
			camPos[X]+=0.2f;
			break;
	}
}

//menu stuff
void menuProc(int value){
	if (value == 1)
		printf("First item was clicked\n");
}

void createOurMenu(){
	//int subMenu_id = glutCreateMenu(menuProc2);
	int subMenu_id = glutCreateMenu(menuProc);
	glutAddMenuEntry("gahh", 3);
	glutAddMenuEntry("gahhhhhh", 4);
	glutAddMenuEntry("????", 5);
	glutAddMenuEntry("!!!!!", 6);

	int main_id = glutCreateMenu(menuProc);
	glutAddMenuEntry("First item", 1);
	glutAddMenuEntry("Second item", 2);
	glutAddSubMenu("pink flamingo", subMenu_id);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)((w+0.0f)/h), 1, 300);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void FPSTimer(int value){ //60fps
	glutTimerFunc(17, FPSTimer, 0);
	glutPostRedisplay();
}

//initialization
void init(void)
{
	glClearColor(0.9, 0.9, 0.9, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)((glutGet(GLUT_WINDOW_WIDTH)+0.0f)/glutGet(GLUT_WINDOW_HEIGHT)), 1, 300);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	float lPos0[4] = {2, 2, 0, 0};
	float ambi0[4] = {0.9, 0.9, 0.9, 1};
	float diff0[4] = {0.9, 0.9, 0.9, 1};
	float spec0[4] = {0.1, 0.1, 0.1, 0};

	glLightfv(GL_LIGHT0, GL_POSITION, lPos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

	currentPlane = new Plane();
	currentPlane->InitPlane(-0.001, 1, 0, 32, 0, 0, 0);
	// currentPlane->LaunchPlane();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(900, 600);
	glutInitWindowPosition(100, 50);

	glutCreateWindow("Project");	//creates the window

	//display callback
	glutDisplayFunc(display);

	//keyboard callback
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	//mouse callbacks
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mousePassiveMotion);

	//resize callback
	glutReshapeFunc(reshape);

	//fps timer callback
	glutTimerFunc(17, FPSTimer, 0);

	init();

	createOurMenu();

	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
