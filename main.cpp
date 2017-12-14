#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//Globals
float camPos[] = {0, 0, 3.42f};	//where the camera is

// Welcome Screen
bool welcome = false;
bool end = false;

//OpenGL functions
void keyboard(unsigned char key, int xIn, int yIn)
{
	switch (key)
	{
		case 'q':
		case 27:	//27 is the esc key
			exit(0);
			break;
	}
}

void mouse(int btn, int state, int mouseX, int mouseY)
{
	if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		welcome = true;
	}
	else if(btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		end = true;
	}
}

void drawBitmapText(char *string) 
{  
	char *c;
	//glRasterPos3f(x, y,z);

	for (c=string; *c != '\0'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}


void init(void)
{
	glClearColor(1, 1, 1, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);
}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0,1,0);
	
	if(welcome == false)
	{	
		glColor3f(1,0,0);
		glutSolidCube(100);
	
		drawBitmapText("Welcome to Paper Plane Wars. Right click to start.");
	}
	
	else if(welcome == true && end == false)
	{
		glColor3f(0,1,0);
		glutSolidTeapot(1);
	}
	
	else if(welcome == true && end == true)
	{
		glColor3f(0,0,0);
		glutSolidCube(17);

		drawBitmapText("Game Over.");
	}

	glFlush();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv); 	
	glutInitDisplayMode(GLUT_RGBA);	

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(600, 500);

	glutCreateWindow("Project");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glutDisplayFunc(display);
	
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);

	init();
	glutMainLoop();	
	return(0);					//return may not be necessary on all compilers
}
