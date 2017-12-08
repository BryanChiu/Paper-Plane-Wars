#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <algorithm>
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

#include "Plane.h"

Plane::Plane() {}

// initialize 2D array of heights
void Plane::InitPlane(float gravin, float scalin, float xin, float yin, float zin, float xang, float yang) {
	gravity = gravin;
	airFriction = 0.995;
	scale = scalin;

	pos.push_back(xin);
	pos.push_back(yin);
	pos.push_back(zin);

	pitch = xang;
	yaw = yang;

	inFlight = false;

}

// render terrain
void Plane::DrawPlane() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_FRONT); // ONLY FOR TEAPOT

	// material info for terrain (grassy hills...kind of)
	float m_spec[4] = {0.99, 0.98, 0.81,1};
	float m_shin = 10;
	float m_diff[4] = {1, 0, 0, 1};
	float m_ambi[4] = {0.3, 0, 0, 1};
		
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambi);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_shin);

	// enable/disable lighting
	if (true) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}

	glRotatef(90, 0, 1, 0);
	glutSolidTeapot(1);
}

void Plane::MovePlane() {
	pos[0] += vel[0];
	pos[1] += vel[1];
	pos[2] += vel[2];

	vel[0] *= airFriction; // loss of momentum
	vel[1] *= airFriction;
	vel[2] *= airFriction;

	vel[1] += gravity;

	if (pos[1]<=0) {
		pos[1] = 0;
		vel[1] *= -1;
	}
}

/*
void Plane::AimPlane(int xin, int yin) {
	int rightBound = glutGet(GLUT_WINDOW_WIDTH)/2+200;
	int leftBound = glutGet(GLUT_WINDOW_WIDTH)/2-200;
	int topBound = glutGet(GLUT_WINDOW_HEIGHT)/2;
	int bottomBound = glutGet(GLUT_WINDOW_HEIGHT)/2 + 200;

	// setting min/max rotations
	int pitchMin = 0;
	int pitchMax = 25;
	int yawMin = -45;
	int yawMax = 45;

	// restricting angles
	if (xin<leftBound)
		xin = leftBound;
	if (xin>rightBound)
		xin = rightBound;
	if (yin<topBound)
		yin = topBound;
	if (yin>bottomBound)
		yin = bottomBound;

	// mapping 2D window coords to min/max angles
	yaw = yawMin + (xin - leftBound)*(yawMax - yawMin)/(rightBound - leftBound);
	pitch = pitchMin + (yin - topBound)*(pitchMax - pitchMin)/(bottomBound - topBound);
}
*/

void Plane::SetPitch(int timerIn) {
	pitch = abs(timerIn)*0.5;
}

void Plane::SetYaw(int timerIn) {
	yaw = abs(timerIn)-25;
}

void Plane::SetPower(int timerIn) {
	power = 0.6/pow(abs(timerIn), 1.0/3.0) + 0.2;

	if (timerIn==0)
		power = 0.8;
}

void Plane::LaunchPlane() {
	std::vector<float> velTemp;

	velTemp.push_back(-sin(yaw*3.14159/180)/sin(90-abs(yaw*3.14159/180)));
	velTemp.push_back(sin(pitch*3.14159/180)/sin(90-pitch*3.14159/180));
	velTemp.push_back(-1);

	float normalMagn = sqrt(velTemp[0]*velTemp[0] + velTemp[1]*velTemp[1] + velTemp[2]*velTemp[2]);	

	vel.push_back(power*velTemp[0]/normalMagn);
	vel.push_back(power*velTemp[1]/normalMagn);
	vel.push_back(power*velTemp[2]/normalMagn);

	std::ofstream fileout;
	fileout.open("launchdata.txt");
	fileout << "Pitch: "<<pitch<<"\nYaw: "<<yaw<<"\nPower: "<<power;
	fileout.close();
}

// returns cross product, used in CalculateSurfaceNormals
std::vector<float> Plane::getCoords() {
	return pos;
}

std::vector<float> Plane::getOrient() {
	std::vector<float> orient;

	orient.push_back(pitch);
	orient.push_back(yaw);

	return orient;
}

float Plane::getPower() {
	return power;
}