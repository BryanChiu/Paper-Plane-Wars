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
Plane *currentPlane;

#define X 0
#define Y 1
#define Z 2
#define W 3

float camPos[] = {0, 35, 10};	//where the camera is
float camTarget[] = {0, 34, 5};

int launchState = 0; // 0=none, 1=pitch, 2=yaw, 3=power
int wheelTimer;

//// plane obj ////
// std::vector<float> vertices;
// std::vector<float> uvs;
// std::vector<float> normals;

void FloorMesh() {
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCullFace(GL_BACK);

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

// void DrawOBJPlane() {
// 	glEnable(GL_LIGHTING);
// 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// 	glDisable(GL_CULL_FACE);

// 	glBegin(GL_TRIANGLES);
// 		for (int i=0; i<vertices.size(); i+=3) {
// 			glNormal3f(normals[i], normals[i+1], normals[i+2]);
// 			glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
// 		}
// 	glEnd();

// 	glEnable(GL_CULL_FACE);
// }

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
			// DrawOBJPlane();
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

////////vvvv  OBJ FILE LOADER vvvv////////

// bool loadOBJ(const char * path, std::vector<float> &out_vertices, std::vector<float> &out_uvs, std::vector<float> &out_normals) {
// 	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices; // vertex/uv/normal format in each line starting with 'f'
// 	std::vector<float> temp_vertices;
// 	std::vector<float> temp_uvs;
// 	std::vector<float> temp_normals;

// 	FILE * file = fopen(path, "r");
// 	if( file == NULL ){
// 		printf("Impossible to open the file !\n");
// 		return false;
// 	}

// 	while( 1 ){
// 		char lineHeader[128];
// 		// read the first word of the line
// 		int res = fscanf(file, "%s", lineHeader);
// 		if (res == EOF)
// 			break;

// 		if ( strcmp( lineHeader, "v" ) == 0 ){
// 			float vertex1, vertex2, vertex3;
// 			fscanf(file, "%f %f %f\n", &vertex1, &vertex2, &vertex3 );
// 			temp_vertices.push_back(vertex1);
// 			temp_vertices.push_back(vertex2);
// 			temp_vertices.push_back(vertex3);
// 		} else if ( strcmp( lineHeader, "vt" ) == 0 ){
// 			float uv1, uv2;
// 			fscanf(file, "%f %f\n", &uv1, &uv2 );
// 			temp_uvs.push_back(uv1);
// 			temp_uvs.push_back(uv2);
// 		} else if ( strcmp( lineHeader, "vn" ) == 0 ){
// 			float normal1, normal2, normal3;
// 			fscanf(file, "%f %f %f\n", &normal1, &normal2, &normal3 );
// 			temp_normals.push_back(normal1);
// 			temp_normals.push_back(normal2);
// 			temp_normals.push_back(normal3);
// 		}else if ( strcmp( lineHeader, "f" ) == 0 ){
// 			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
// 			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
// 			if (matches != 9){
// 				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
// 				return false;
// 			}
// 			vertexIndices.push_back(vertexIndex[0]);
// 			vertexIndices.push_back(vertexIndex[1]);
// 			vertexIndices.push_back(vertexIndex[2]);
// 			uvIndices.push_back(uvIndex[0]);
// 			uvIndices.push_back(uvIndex[1]);
// 			uvIndices.push_back(uvIndex[2]);
// 			normalIndices.push_back(normalIndex[0]);
// 			normalIndices.push_back(normalIndex[1]);
// 			normalIndices.push_back(normalIndex[2]);
// 		}
// 	}

// 	for(int i=0; i<vertexIndices.size(); i++) {
// 		out_vertices.push_back(temp_vertices[(vertexIndices[i]-1)*3]);
// 		out_vertices.push_back(temp_vertices[(vertexIndices[i]-1)*3+1]);
// 		out_vertices.push_back(temp_vertices[(vertexIndices[i]-1)*3+2]);
// 	}
// 	for(int i=0; i<uvIndices.size(); i++) {
// 		out_uvs.push_back(temp_uvs[(uvIndices[i]-1)*3]);
// 		out_uvs.push_back(temp_uvs[(uvIndices[i]-1)*3+1]);
// 		out_uvs.push_back(temp_uvs[(uvIndices[i]-1)*3+2]);
// 	}
// 	for(int i=0; i<normalIndices.size(); i++) {
// 		out_normals.push_back(temp_normals[(normalIndices[i]-1)*3]);
// 		out_normals.push_back(temp_normals[(normalIndices[i]-1)*3+1]);
// 		out_normals.push_back(temp_normals[(normalIndices[i]-1)*3+2]);
// 	}
// }

////////^^^^  OBJ FILE LOADER ^^^^////////

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

	// bool wut = loadOBJ("paper_plane.obj", vertices, uvs, normals);
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
