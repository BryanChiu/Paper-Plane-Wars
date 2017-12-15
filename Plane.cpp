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
void Plane::InitPlane(int id, char *file, float gravin, float xin, float yin, float zin, float xang, float yang) {
	ID = id;
	gravity = gravin;
	airFriction = 0.995;

	pos.push_back(xin);
	pos.push_back(yin);
	pos.push_back(zin);

	pitch = xang;
	yaw = yang;

	inFlight = false;

	bool wut = LoadOBJ(file, vertices, uvs, normals);

	// create bounding box
	for (int i=0; i<3; i++) {
		float max = 0.0;
		float min = 0.0;
		for (int j=i; j<vertices.size(); j+=3) {
			if (vertices[j]>max) {
				max = vertices[j];
			}
			if (vertices[j]<min) {
				min = vertices[j];
			}
		}
		boundMax.push_back(max);
		boundMin.push_back(min);
	}

	// create face normals
	for (int i=-1; i<2; i+=2) {
		for (int j=0; j<3; j++) {
			std::vector<float> normVec (3, 0);
			normVec[j] = i;
			faceNorms.push_back(normVec);
		}
	}
}

bool Plane::LoadOBJ(const char * path, std::vector<float> &out_vertices, std::vector<float> &out_uvs, std::vector<float> &out_normals) {
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices; // vertex/uv/normal format in each line starting with 'f'
	std::vector<float> temp_vertices;
	std::vector<float> temp_uvs;
	std::vector<float> temp_normals;

	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file !\n");
		exit(0);
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
		} else if ( strcmp( lineHeader, "f" ) == 0 ){
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
		} else if ( strcmp( lineHeader, "m1" ) == 0 ) {
			float ambi1, ambi2, ambi3, ambi4, diff1, diff2, diff3, diff4, spec1, spec2, spec3, spec4, shin;
			fscanf(file, "%f %f %f %f, %f %f %f %f, %f %f %f %f, %f\n", &ambi1, &ambi2, &ambi3, &ambi4, &diff1, &diff2, &diff3, &diff4, &spec1, &spec2, &spec3, &spec4, &shin);
			materialStruct tempMat = {{ambi1, ambi2, ambi3, ambi4}, {diff1, diff2, diff3, diff4}, {spec1, spec2, spec3, spec4}, shin};
			materialMain = tempMat;
		} else if ( strcmp( lineHeader, "m2" ) == 0 ) {
			float ambi1, ambi2, ambi3, ambi4, diff1, diff2, diff3, diff4, spec1, spec2, spec3, spec4, shin;
			fscanf(file, "%f %f %f %f, %f %f %f %f, %f %f %f %f, %f\n", &ambi1, &ambi2, &ambi3, &ambi4, &diff1, &diff2, &diff3, &diff4, &spec1, &spec2, &spec3, &spec4, &shin);
			materialStruct tempMat = {{ambi1, ambi2, ambi3, ambi4}, {diff1, diff2, diff3, diff4}, {spec1, spec2, spec3, spec4}, shin};
			materialSecondary = tempMat;
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
void Plane::DrawPlane(bool active) {
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialMain.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialMain.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialMain.specular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialMain.shininess);

	if (!active) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, materialSecondary.ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, materialSecondary.diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, materialSecondary.specular);
		glMaterialf(GL_FRONT, GL_SHININESS, materialSecondary.shininess);
	}

	glBegin(GL_TRIANGLES);
		int j=0;
		for (int i=0; i<vertices.size(); i+=3) {
			glNormal3f(normals[i], normals[i+1], normals[i+2]);
			glTexCoord2f(uvs[j], uvs[j+1]);
			glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
			j+=2;
		}
	glEnd();

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

void Plane::ExhibitPlane(int timerIn) {
	glRotatef(-timerIn, 0, 1, 0);
	glScalef(1.1, 1.1, 1.1);
}

void Plane::MovePlane() {
	if (!inFlight) return;

	pos[0] += vel[0];
	pos[1] += vel[1];
	pos[2] += vel[2];

	vel[0] *= airFriction; // loss of momentum
	vel[1] *= airFriction;
	vel[2] *= airFriction;

	vel[1] += gravity;

	if (pos[1]<=0) {
		inFlight = false;
	}

	float velMag = sqrt(vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2]);

	yaw = -asin(vel[0]/velMag)*180/PI;

	pitch = asin(vel[1]/velMag)*180/PI;
	if (pitch<0) {
		pitch = 0;
	}

	if (abs(roll)>0.2) {
		roll-=(abs(roll)/roll)*0.2;
	}
}

void Plane::SetPitch(int timerIn) { // pitch is between 0 - 25
	pitch = abs(timerIn)*0.5;
}

void Plane::SetYaw(int timerIn) { // yaw is between -25 - 25
	yaw = abs(timerIn)-25;
}

void Plane::SetPower(int timerIn) { // power is between 0.37 - 0.8
	power = 0.6/pow(abs(timerIn), 0.33) + 0.2;

	if (timerIn==0)
		power = 0.8;
}

void Plane::LaunchPlane() {
	std::vector<float> velTemp;

	velTemp.push_back(-sin(yaw*PI/180)/sin(90-abs(yaw*PI/180)));
	velTemp.push_back(sin(pitch*PI/180)/sin(90-pitch*PI/180));
	if (abs(yaw)<50) {
		velTemp.push_back(-1);
	} else {
		velTemp.push_back(1);
	}

	float normalMagn = sqrt(velTemp[0]*velTemp[0] + velTemp[1]*velTemp[1] + velTemp[2]*velTemp[2]);	

	vel.push_back(power*velTemp[0]/normalMagn);
	vel.push_back(power*velTemp[1]/normalMagn);
	vel.push_back(power*velTemp[2]/normalMagn);

	std::ofstream fileout;
	fileout.open("launchdata.txt", std::ios_base::app);
	fileout << "Pitch: "<<pitch<<"\nYaw: "<<yaw<<"\nPower: "<<power<<"\n\n";
	fileout.close();
}

void Plane::BlowPlane(float xin, float yin) {
	if (!inFlight) return;

	float velMag2D = sqrt(vel[0]*vel[0] + vel[2]*vel[2]);

	if (velMag2D>0.01) {
		vel[0]+=xin;
		vel[1]+=yin;
		vel[2] = -sqrt(velMag2D*velMag2D - vel[0]*vel[0]);
		roll+=-xin*1000;
		yaw+=-xin*500;
	}
}

void Plane::Collision() {
	inFlight= false;
}

int Plane::getID() {
	return ID;
}

std::vector<float> Plane::getCoords() {
	return pos;
}

std::vector<float> Plane::getOrient() {
	std::vector<float> orient;

	orient.push_back(pitch);
	orient.push_back(yaw);
	orient.push_back(roll);

	return orient;
}

float Plane::getPower() {
	return power;
}

std::vector< std::vector<float> > Plane::getBoundFaceNorms() {
	return faceNorms;
}

std::vector<float> Plane::getBoundFaceDists() {
	std::vector<float> faceDists;

	// distances are simply the bounding box coords
	// max values are returned negative as required by ray-plane intersect equation
	faceDists.push_back(boundMin[0]);
	faceDists.push_back(boundMin[1]);
	faceDists.push_back(boundMin[2]);
	faceDists.push_back(-boundMax[0]);
	faceDists.push_back(-boundMax[1]);
	faceDists.push_back(-boundMax[2]);

	return faceDists;
}