#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <iostream>
#include <fstream>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include "Human.h"

Human::Human() {}

// initialize 2D array of heights
void Human::InitHuman(int id, float xin, float yin, float zin) {
	ID = id;
	health = 3;

	pos.push_back(xin);
	pos.push_back(yin);
	pos.push_back(zin);

	rot.push_back(0);
	rot.push_back(0);
	rot.push_back(0);

	// create bounding box
	boundMin.push_back(-1.25);
	boundMin.push_back(-3.0);
	boundMin.push_back(-1);
	boundMax.push_back(1.25);
	boundMax.push_back(2.25);
	boundMax.push_back(1);
	
}

// render terrain
void Human::DrawHuman() {
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);

	glTranslatef(pos[0], pos[1], pos[2]);

	glPushMatrix(); //body and head and arms rotation
		glRotatef(rot[0],1,0,0);
		glRotatef(rot[1],0,1,0);		
		glRotatef(rot[2],0,0,1);
		glPushMatrix();//body size
			glScalef(1,2,1);
			glColor3f(0,1,0);
			glutSolidCube(1);
		glPopMatrix();//end body size
		glPushMatrix();//head location
			glScalef(0.75,0.75,0.75);
			glTranslatef(0,2,0);
			glColor3f(0.6,0.1,0.1);
			glutSolidSphere(1,20,20); 
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

	// SHOW BOUNDING BOX
		glDisable(GL_LIGHTING); // disable lighting to allow flat green colour
		glBegin(GL_LINES);
			glColor3f(0.5, 1, 0.5);
			glVertex3f(boundMin[0], boundMin[1], boundMin[2]);
			glVertex3f(boundMin[0], boundMin[1], boundMax[2]);
			glVertex3f(boundMin[0], boundMin[1], boundMin[2]);
			glVertex3f(boundMin[0], boundMax[1], boundMin[2]);
			glVertex3f(boundMin[0], boundMin[1], boundMin[2]);
			glVertex3f(boundMax[0], boundMin[1], boundMin[2]);

			glVertex3f(boundMin[0], boundMax[1], boundMax[2]);
			glVertex3f(boundMin[0], boundMax[1], boundMin[2]);
			glVertex3f(boundMin[0], boundMax[1], boundMax[2]);
			glVertex3f(boundMin[0], boundMin[1], boundMax[2]);
			glVertex3f(boundMin[0], boundMax[1], boundMax[2]);
			glVertex3f(boundMax[0], boundMax[1], boundMax[2]);

			glVertex3f(boundMax[0], boundMax[1], boundMin[2]);
			glVertex3f(boundMax[0], boundMax[1], boundMax[2]);
			glVertex3f(boundMax[0], boundMax[1], boundMin[2]);
			glVertex3f(boundMax[0], boundMin[1], boundMin[2]);
			glVertex3f(boundMax[0], boundMax[1], boundMin[2]);
			glVertex3f(boundMin[0], boundMax[1], boundMin[2]);

			glVertex3f(boundMax[0], boundMin[1], boundMax[2]);
			glVertex3f(boundMax[0], boundMin[1], boundMin[2]);
			glVertex3f(boundMax[0], boundMin[1], boundMax[2]);
			glVertex3f(boundMax[0], boundMax[1], boundMax[2]);
			glVertex3f(boundMax[0], boundMin[1], boundMax[2]);
			glVertex3f(boundMin[0], boundMin[1], boundMax[2]);
		glEnd();
		glEnable(GL_LIGHTING);
}

void Human::DodgePlane(std::vector<float> planePos) {
	//change position to dodge plane according to x plane position
	if(pos[2]-planePos[2] <= 50 && pos[1]-planePos[1] <= 30){
		if(planePos[0] > pos[0]){
			if(rot[2]<40){
				rot[2]++;
			}
		}
		else if(planePos[0] <= pos[0]){
			if(rot[2]>-40){
				rot[2]--;
			}
		}
	}
	//when far enough, z plane position, go back to standing
	else {
		if(rot[2]>0){
			rot[2]--;
		}
		else if(rot[2]<0){
			rot[2]++;
		}		
	}
}

void Human::TakeDamage() {
	health--;
}

void Human::MoveHuman(float xin) {
	pos[0]+=xin;
}

int Human::getID() {
	return ID;
}

std::vector<float> Human::getCoords() {
	return pos;
}

std::vector<float> Human::getHitBox() {
	std::vector<float> hitbox;
	hitbox.push_back(boundMin[0]);
	hitbox.push_back(boundMin[1]);
	hitbox.push_back(boundMin[2]);
	hitbox.push_back(boundMax[0]);
	hitbox.push_back(boundMax[1]);
	hitbox.push_back(boundMax[2]);

	return hitbox;
}

std::vector<float> Human::getOrient() {
	return rot;
}

int Human::getHealth() {
	return health;
}