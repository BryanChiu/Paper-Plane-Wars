#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include <string.h>

//Camera Position
float camPos[] = {0, 10, 15};

// Welcome Screen
bool welcome = false;

// Function to display text on screen
void drawText(char *text,float xText,float yText,float zText) 
{  
	char *cText;
	glRasterPos3f(xText, yText, zText);

	for (cText=text; *cText != '\0'; cText++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *cText);
	}
}

void mouse(int btn, int state, int mouseX, int mouseY)
{
	if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			welcome = true;
		}
}
 
// Game initialization
void init(void){
	glClearColor(1, 1, 1, 0);

	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();			
	gluPerspective(45, 1, 1, 100);
}

// Main display function
void display(void){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0, 1, 0);

	if(welcome == true){
		glColor3f(1,0,0);
		glutSolidTeapot(3);
	}

	if(welcome == false)
	{
		glColor3f(1,1,1);
		drawText("Welcome to Paper Plane Wars. Right click to start.",200,200,0);
	
		glColor3f(0,0,0);
		glutSolidCube(17);
	}

	glutSwapBuffers();
	glutPostRedisplay();
	glFlush();
}

// Main game loop
int main(int argc, char** argv){
	glutInit(&argc, argv); 	
	glutInitDisplayMode(GLUT_RGBA);	

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(400, 400);

	glutCreateWindow("Project");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glutDisplayFunc(display);

	glutMouseFunc(mouse);

	init();
	glutMainLoop();	
	return(0);
}
