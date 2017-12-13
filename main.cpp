#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif


#define X 0
#define Y 1
#define Z 2
#define W 3

float camPos[] = {0, 35, 10};	//where the camera is
float camTarget[] = {0, 34, 5};

int launchState = 0; // 0=none, 1=pitch, 2=yaw, 3=power
int wheelTimer;


//an array for iamge data
GLubyte* snail_tex;
GLuint textures[2];
int width, height, maxi;
int CURRENT = 0;

int bodyRotAngle,bodyRotX,bodyRotY = 0;
int bodyRotZ = 1 ;

void getHitAnim(){
	bodyRotAngle++;
}


void DrawPerson(){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1,1,1);
	//glEnable(GL_LIGHTING);
	glPushMatrix();//person position
		glTranslatef(0,30,0);
		glPushMatrix(); //body and head and arms rotation
			glRotatef(bodyRotAngle,bodyRotX,bodyRotY,bodyRotZ);
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
GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
	GLubyte* img;
	FILE *fd;
	int n, m;
	int  k, nm;
	char c;
	int i;
	char b[100];
	float s;
	int red, green, blue;
	
	fd = fopen(file, "r");
	fscanf(fd,"%[^\n] ",b);
	if(b[0]!='P'|| b[1] != '3')
	{
		printf("%s is not a PPM file!\n",file); 
		exit(0);
	}
	printf("%s is a PPM file\n", file);
	fscanf(fd, "%c",&c);
	while(c == '#') 
	{
		fscanf(fd, "%[^\n] ", b);
		printf("%s\n",b);
		fscanf(fd, "%c",&c);
	}
	ungetc(c,fd); 
	fscanf(fd, "%d %d %d", &n, &m, &k);

	printf("%d rows  %d columns  max value= %d\n",n,m,k);

	nm = n*m;

	img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);


	s=255.0/k;


	for(i=0;i<nm;i++) 
	{
		fscanf(fd,"%d %d %d",&red, &green, &blue );
		img[3*nm-3*i-3]=red*s;
		img[3*nm-3*i-2]=green*s;
		img[3*nm-3*i-1]=blue*s;
	}

	*width = n;
	*height = m;
	*max = k;

	return img;
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

	//check rotation between -90 and 90
	if (bodyRotAngle > 90){
		bodyRotAngle = 90;
	}
	else if (bodyRotAngle < -90){
		bodyRotAngle = -90;
	}
	DrawPerson();

	//flush out to single buffer
	glutSwapBuffers();
}


//OpenGL functions
//mouse
void mouse(int btn, int state, int x, int y){

}

void mouseMotion(int x, int y){
}

void mousePassiveMotion(int x, int y){
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

		case 'n':
			bodyRotAngle-=1;
			break;

		case 'm':
			bodyRotAngle+=1;
			break;
		
		case '0': //get hit
			getHitAnim();
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

void initTextures(){
	//enable texturing
	glEnable(GL_TEXTURE_2D);
	
		//generate 2 texture IDs, store them in array "textures"
		glGenTextures(2, textures);
	
		//load the texture (snail)
		snail_tex = LoadPPM("snail_a.ppm", &width, &height, &maxi);
	
		//setup first texture (using snail image)
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		//set texture parameters
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		//create a texture using the "snail_tex" array data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, snail_tex);
		
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

	
	initTextures();
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
