#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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

#define X 0
#define Y 1
#define Z 2
#define W 3

float camPos[] = {0, 35, 10};	//where the camera is
float camTarget[] = {0, 34, 5};

std::vector<Plane*> PlaneList;
int selectedPlane = -1; // selected object ID, -1 if nothing selected
int highlight = -1;
int highlightTimer = 0;

int launchState = 0; // 0=none, 1=pitch, 2=yaw, 3=power, 4=inFlight
int wheelTimer;

bool select = false;
double* m_start = new double[3]; // ray-casting coords
double* m_end = new double[3];
int _X, _Y;

//character body rotation positions
float bodyRotX,bodyRotY,bodyRotZ = 0;
//opponent rotation positions
float oppRotX, oppRotY, oppRotZ = 0;

//plane position 
std::vector<float> planePos;
//hit player?
bool hit = false;

void DrawOpponent(){
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1,1,1);
	if(launchState==4){
		printf("planePos[0] %f\n",planePos[0]);
		printf("planePos[1] %f\n",planePos[1]);
		printf("planePos[2] %f\n",planePos[2]);
		//change position to dodge plane
		//if(planePos[0] < 50){
		//	oppRotZ = 90;
		//}
	}
	//glEnable(GL_LIGHTING);
	glPushMatrix();//person position
		//NEED TO TRANSLATE THIS SOMEWHERE BETTER
		glTranslatef(0,10,-60);
		glPushMatrix(); //body and head and arms rotation
			glRotatef(oppRotX,1,0,0);
			glRotatef(oppRotY,0,1,0);		
			glRotatef(oppRotZ,0,0,1);
			glPushMatrix();//body size
				glScalef(1,2,1);
				glColor3f(0,1,0);
				glutSolidCube(1);
			glPopMatrix();//end body size
			glPushMatrix();//head location
				glScalef(0.75,0.75,0.75);
				glTranslatef(0,2,0);
				glColor3f(0.6,0.1,0.1);
				glutSolidSphere(1,100,1000); 
				//glBindTexture(GL_TEXTURE_2D, textures[CURRENT]);
				//glutSolidTeapot(1);
			glPopMatrix();//end head loc
			glPushMatrix();//arm size
				glScalef(0.5,2,0.5);
				glPushMatrix(); //left arm position
					glColor3f(0,1,0);
					glTranslatef(-1.5,0,0);
					glRotatef(0,0,0,1);
					glutSolidCube(1);
				glPopMatrix();//end left arm loc
				glPushMatrix();//right arm loc
					glColor3f(0,1,0);
					glTranslatef(1.5,0,0);
					glRotatef(0,0,0,1);
					glutSolidCube(1);
				glPopMatrix();//end right arm loc
			glPopMatrix();//end arm size
		glPopMatrix();//head and body and arms rotation
		glPushMatrix();//legs size
			glScalef(0.5,2,1);
			glPushMatrix();//right leg location
				glColor3f(0,0,0);
				glTranslatef(0.5,-1,0);
				glutSolidCube(1);
			glPopMatrix();//end leg loc
			glPushMatrix();//left leg location
				glColor3f(0,0,0);
				glTranslatef(-0.5,-1,0);
				glutSolidCube(1);
			glPopMatrix();//end leg loc
		glPopMatrix();//end leg and arm size
	glPopMatrix(); //end person loc
}



void DrawPerson(){
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1,1,1);
	//glEnable(GL_LIGHTING);
	glPushMatrix();//person position
		glTranslatef(0,10,0);
		glPushMatrix(); //body and head and arms rotation
			glRotatef(bodyRotX,1,0,0);
			glRotatef(bodyRotY,0,1,0);		
			glRotatef(bodyRotZ,0,0,1);
			glPushMatrix();//body size
				glScalef(1,2,1);
				glColor3f(1,0,0);
				glutSolidCube(1);
			glPopMatrix();//end body size
			glPushMatrix();//head location
				glScalef(0.75,0.75,0.75);
				glTranslatef(0,2,0);
				glColor3f(0.6,0.1,0.1);
				glutSolidSphere(1,100,1000); 
				//glBindTexture(GL_TEXTURE_2D, textures[CURRENT]);
				//glutSolidTeapot(1);
			glPopMatrix();//end head loc
			glPushMatrix();//arm size
				glScalef(0.5,2,0.5);
				glPushMatrix(); //left arm position
					glColor3f(1,0,0);
					glTranslatef(-1.5,0,0);
					glRotatef(0,0,0,1);
					glutSolidCube(1);
				glPopMatrix();//end left arm loc
				glPushMatrix();//right arm loc
					glColor3f(1,0,0);
					glTranslatef(1.5,0,0);
					glRotatef(0,0,0,1);
					glutSolidCube(1);
				glPopMatrix();//end right arm loc
			glPopMatrix();//end arm size
		glPopMatrix();//head and body and arms rotation
		glPushMatrix();//legs size
			glScalef(0.5,2,1);
			glPushMatrix();//right leg location
				glColor3f(0,0,1);
				glTranslatef(0.5,-1,0);
				glutSolidCube(1);
			glPopMatrix();//end leg loc
			glPushMatrix();//left leg location
				glColor3f(0,0,1);
				glTranslatef(-0.5,-1,0);
				glutSolidCube(1);
			glPopMatrix();//end leg loc
		glPopMatrix();//end leg and arm size
	glPopMatrix(); //end person loc
}

void FloorMesh() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);
	glLineWidth(1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//material
	float m_amb0[4] = {0.0,0.05,0.0,1}; //ambient light
	float m_diff0[3] = {0.4,0.5,0.4}; //shadows casting
	float m_spec0[3] = {0.04,0.7,0.04};
	float shine = 0.078125;
	
    glMaterialfv(GL_FRONT,GL_AMBIENT,m_amb0);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,m_diff0);
	glMaterialfv(GL_FRONT,GL_SPECULAR,m_spec0);
	glMaterialf(GL_FRONT, GL_SHININESS, shine * 128.0);

	for (int i=0; i>-100-1; i--) {
		glBegin(GL_QUAD_STRIP);
			for (int j=50; j>-50; j--) {
				glColor3f(0,0,0);
				glVertex3f(j, 0, i);
				glVertex3f(j, 0, i-1);
			}
		glEnd();
	}
}
void hitTimer(int value){
	if (hit){
		if(bodyRotX<30){
			bodyRotX++;
		}
		else if(bodyRotX=30){
			bodyRotX=0;
			hit = false;
			//lose a life
		}
	}
	glutTimerFunc(20, hitTimer, 0);
	glutPostRedisplay();
}
void LaunchSequence(std::vector<Plane*>::iterator obj) {
	glDisable(GL_LIGHTING);
	wheelTimer++;
	if (wheelTimer>50) {
		wheelTimer = -50;
	}

	switch (launchState) {
		case 1:
			(*obj)->SetPitch(wheelTimer);
			break;

		case 2:
			(*obj)->SetYaw(wheelTimer);

			break;

		case 3:
			(*obj)->SetPower(wheelTimer);
			break;
	}

	// Show launch vector
	glLineWidth(5);
	glBegin(GL_LINES);
		glColor3f(0,0,0);
		glVertex3f(0,0,3);
		if (launchState==3) {
			glVertex3f(0,0,(*obj)->getPower()*-20 + 5);
		} else {
			glVertex3f(0,0,-16);
		}
	glEnd();
	glLineWidth(1);
}

//OpenGL functions
//mouse
void mouse(int btn, int state, int x, int y){
	int centerX = glutGet(GLUT_WINDOW_WIDTH)/2;
	int centerY = glutGet(GLUT_WINDOW_HEIGHT)/2;

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN && launchState == 0){
		select = true;
		_X = x;
		_Y = y;
	}
}

std::vector<float> RayTest(int objID, std::vector<Plane*>::iterator obj) {
	double matModelView[16], matProjection[16];
	int viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	double winX = (double)_X;
	double winY = viewport[3] - (double)_Y;

	gluUnProject(winX, winY, 0.0, matModelView, matProjection, viewport, &m_start[X], &m_start[Y], &m_start[Z]);
	gluUnProject(winX, winY, 1.0, matModelView, matProjection, viewport, &m_end[X], &m_end[Y], &m_end[Z]);

	double* R0 = new double[3];
	double* Rd = new double[3];

	double xDiff = m_end[X] - m_start[X];
	double yDiff = m_end[Y] - m_start[Y];
	double zDiff = m_end[Z] - m_start[Z];

	double mag = sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff);
	R0[X] = m_start[X];
	R0[Y] = m_start[Y];
	R0[Z] = m_start[Z];

	Rd[X] = xDiff / mag;
	Rd[Y] = yDiff / mag;
	Rd[Z] = zDiff / mag;

	std::vector<float> intersectRet;
	std::vector< std::vector<float> > faceNorms = (*obj)->getBoundFaceNorms();
	std::vector<float> faceDists = (*obj)->getBoundFaceDists();

	for (int i=0; i<6; i++) {
		double NRd = faceNorms[i][X]*Rd[X] + faceNorms[i][Y]*Rd[Y] + faceNorms[i][Z]*Rd[Z];

		// only count intersection with front of face (only testing 3 faces)
		if (NRd < 0) {
			double NR0 = faceNorms[i][X]*R0[X] + faceNorms[i][Y]*R0[Y] + faceNorms[i][Z]*R0[Z];
			double t = -(NR0 + faceDists[i]) / NRd;

			double point[] = {R0[X] + t*Rd[X], R0[Y] + t*Rd[Y], R0[Z] + t*Rd[Z]};

			// ray distance info (only used when in-bounds test passes)
			double xtoP = R0[X] - point[X];
			double ytoP = R0[Y] - point[Y];
			double ztoP = R0[Z] - point[Z];

			double magtoP = sqrt(xtoP*xtoP + ytoP*ytoP + ztoP*ztoP); // distance from camera to intersection point

			// check if intersection point is within bounds!!!
			if (((i==0 || i==3) && point[Y]>=faceDists[1] && point[Y]<=-faceDists[4] && point[Z]>=faceDists[2] && point[Z]<=-faceDists[5]) ||
				((i==1 || i==4) && point[X]>=faceDists[0] && point[X]<=-faceDists[3] && point[Z]>=faceDists[2] && point[Z]<=-faceDists[5]) ||
				((i==2 || i==5) && point[X]>=faceDists[0] && point[X]<=-faceDists[3] && point[Y]>=faceDists[1] && point[Y]<=-faceDists[4])) {
				// intersection in-bounds! Add distance and object ID to list
				intersectRet.push_back(magtoP);
				intersectRet.push_back((float)objID);
			}
		}
	}

	return intersectRet;
}

// determining closest intersection from list of intersections
void DetermineSelection(std::vector<float> IntersectList) {
	if (IntersectList.size()>0) {
		float minVal = 900.0;
		float minID;

		std::vector<float>::iterator it2 = IntersectList.begin(); 
		while(it2 != IntersectList.end()) {
			//replace minVal with closest intersection
			if (*it2 < minVal) {
				minVal = *it2;
				it2++;
				minID = *it2;
			} else {
				it2++;
			}
			it2++;
		}

		if (minID>=3) {
			return;
		}

		if (highlight == PlaneList[minID]->getID()) { // if the cursor is over the same plane as it was last cycle
			highlightTimer++;
		}
		highlight = PlaneList[minID]->getID();

		if (select && (int)minID!=selectedPlane) {
			selectedPlane = (int)minID;

			while (PlaneList.size()>3) {
				PlaneList.pop_back();
			}
			Plane *newPlane = new Plane();
			char filename[] = "plane_1.obj";
			filename[6] = selectedPlane+'1';
			newPlane->InitPlane(PlaneList.size(), filename, -0.001, 0, 32, 0, 0, 0);
			PlaneList.push_back(newPlane);
		}
	} else { 
		// if no intersections after ray test, reset stuff
		highlight = -1;
		highlightTimer = 0;
	}

	select = false;
}

void mouseMotion(int x, int y){

}

void mousePassiveMotion(int x, int y){
	if (launchState==0) {
		_X = x;
		_Y = y;
	}
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
			if (PlaneList.size()>3) {
				PlaneList.pop_back();
			}
			wheelTimer = -30;
			launchState = 0;
			break;

		case 'n':
			bodyRotZ-=1;
			break;

		case 'm':
			bodyRotZ+=1;
			break;
		
		case '0': //get hit
			hit = true;
			break;	

		case ' ':
			if (PlaneList.size()==4) {
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
						launchState = 4;
						PlaneList[3]->inFlight = true;
						PlaneList[3]->LaunchPlane();
						break;
				}
			}
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

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], camTarget[0], camTarget[1], camTarget[2], 0,1,0);

	FloorMesh();
	DrawPerson();
	DrawOpponent();

	std::vector<float> IntersectList;

	for (std::vector<Plane*>::iterator it = PlaneList.begin(); it != PlaneList.end(); it++) {
		if (launchState==0 || (*it)->getID()>=3) {
			glPushMatrix();
				std::vector<float> translateCoords = (*it)->getCoords();
				glTranslatef(translateCoords[0], translateCoords[1], translateCoords[2]);
				std::vector<float> rotateAngles = (*it)->getOrient();
				glRotatef(rotateAngles[0], 1, 0, 0); // x-axis rotation (pitch)
				glRotatef(rotateAngles[1], 0, 1, 0); // y-axis rotation (yaw)

				glPushMatrix();
					glCullFace(GL_BACK);
					glEnable(GL_LIGHTING);
					glPushMatrix();
						if (highlight == (*it)->getID() && highlight<3) {
							(*it)->ExhibitPlane(highlightTimer);
						}
						(*it)->DrawPlane(highlight == (*it)->getID());
					glPopMatrix();

					if (launchState==0) {
						std::vector<float> intersection = RayTest((*it)->getID(), it);
						if (intersection.size() != 0) {
							IntersectList.push_back(intersection[0]);
							IntersectList.push_back(intersection[1]);
						}
					}
				glPopMatrix();

				if (launchState != 0 && launchState != 4) {
					LaunchSequence(it);
					std::vector<float> tempVec = PlaneList[3]->getCoords();
					for (int i=0; i<3; i++) {
						planePos.push_back(tempVec[i]);
					}
				}

				if ((*it)->inFlight) {
					(*it)->MovePlane();
				}

			glPopMatrix();
		}
	}

	if (launchState==0) {
		DetermineSelection(IntersectList);
	}

	//check rotation between -60 and 60
	if (bodyRotZ > 60){
		bodyRotZ = 60;
	}
	else if (bodyRotZ < -60){
		bodyRotZ = -60;
	}

	//flush out to single buffer
	glutSwapBuffers();
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

	for (int i=0; i<3; i++) {
		Plane *newPlane = new Plane();
		char filename[] = "plane_1.obj";
		filename[6] = i+'1';
		newPlane->InitPlane(PlaneList.size(), filename, -0.001, i*4-4, 30, 0, 0, 0);
		PlaneList.push_back(newPlane);
	}

	remove("launchdata.txt");

	// ray-casting infos
	m_start[X] = 0;
	m_start[Y] = 0;
	m_start[Z] = 0;

	m_end[X] = 0;
	m_end[Y] = 0;
	m_end[Z] = 0;
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
	glutTimerFunc(17, hitTimer, 0);
	

	init();

	createOurMenu();

	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
