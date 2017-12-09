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

#define PI 3.14159265

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

	bool wut = LoadOBJ("paper_plane.obj", vertices, uvs, normals);

}

bool Plane::LoadOBJ(const char * path, std::vector<float> &out_vertices, std::vector<float> &out_uvs, std::vector<float> &out_normals) {
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices; // vertex/uv/normal format in each line starting with 'f'
	std::vector<float> temp_vertices;
	std::vector<float> temp_uvs;
	std::vector<float> temp_normals;

	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file !\n");
		return false;
	}

	while( 1 ){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if ( strcmp( lineHeader, "v" ) == 0 ){
			float vertex1, vertex2, vertex3;
			fscanf(file, "%f %f %f\n", &vertex1, &vertex2, &vertex3 );
			temp_vertices.push_back(vertex1);
			temp_vertices.push_back(vertex2);
			temp_vertices.push_back(vertex3);
		} else if ( strcmp( lineHeader, "vt" ) == 0 ){
			float uv1, uv2;
			fscanf(file, "%f %f\n", &uv1, &uv2 );
			temp_uvs.push_back(uv1);
			temp_uvs.push_back(uv2);
		} else if ( strcmp( lineHeader, "vn" ) == 0 ){
			float normal1, normal2, normal3;
			fscanf(file, "%f %f %f\n", &normal1, &normal2, &normal3 );
			temp_normals.push_back(normal1);
			temp_normals.push_back(normal2);
			temp_normals.push_back(normal3);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for(int i=0; i<vertexIndices.size(); i++) {
		out_vertices.push_back(temp_vertices[(vertexIndices[i]-1)*3]);
		out_vertices.push_back(temp_vertices[(vertexIndices[i]-1)*3+1]);
		out_vertices.push_back(temp_vertices[(vertexIndices[i]-1)*3+2]);
	}
	for(int i=0; i<uvIndices.size(); i++) {
		out_uvs.push_back(temp_uvs[(uvIndices[i]-1)*3]);
		out_uvs.push_back(temp_uvs[(uvIndices[i]-1)*3+1]);
		out_uvs.push_back(temp_uvs[(uvIndices[i]-1)*3+2]);
	}
	for(int i=0; i<normalIndices.size(); i++) {
		out_normals.push_back(temp_normals[(normalIndices[i]-1)*3]);
		out_normals.push_back(temp_normals[(normalIndices[i]-1)*3+1]);
		out_normals.push_back(temp_normals[(normalIndices[i]-1)*3+2]);
	}
}

// render terrain
void Plane::DrawPlane() {
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE); //disable culling as plane has no volume

	// material for plane
	float m_spec[4] = {0.99, 0.98, 0.81,1};
	float m_shin = 10;
	float m_diff[4] = {1, 0, 0, 1};
	float m_ambi[4] = {0.3, 0, 0, 1};
		
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambi);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_shin);


	glBegin(GL_TRIANGLES);
		for (int i=0; i<vertices.size(); i+=3) {
			glNormal3f(normals[i], normals[i+1], normals[i+2]);
			glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
		}
	glEnd();

	glEnable(GL_CULL_FACE);
	// glRotatef(90, 0, 1, 0);
	// glutSolidTeapot(1);
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

	float velMag = sqrt(vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2]);

	pitch = asin(vel[1]/velMag)*180/PI;
	printf("pitch: %f\n", pitch);
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

	velTemp.push_back(-sin(yaw*PI/180)/sin(90-abs(yaw*PI/180)));
	velTemp.push_back(sin(pitch*PI/180)/sin(90-pitch*PI/180));
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