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

void drawBitmapText(char *string,float x,float y,float z) 
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
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

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
	
	drawBitmapText("Welcome to Paper Plane Wars. Right click to start.",200,200,0);

	//flush out to single buffer
	glFlush();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(600, 400);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("3GC3 Boilerplate");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	
	init();

	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
