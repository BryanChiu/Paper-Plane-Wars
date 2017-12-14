#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <vector>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include "Mesh.h"

Mesh::Mesh() {}

// initial generation of terrain heights
void Mesh::GenerateHeights() {
	srand(time(NULL));

	for (int n=0; n<500; n++) {						// iterate random heights 500 times
		AlterAltitude(1, (int)(rand()%sideLength), (int)(rand()%sideLength));
	}
}

// calculate surface normals of each face
void Mesh::CalculateSurfaceNormals() {
	faceNorms = new float**[sideLength-1];			// number of rows of face is one less than number of vertices
	std::vector<float> currFace;

	for (int i=0; i<sideLength-1; i++) {
		faceNorms[i] = new float*[(sideLength-1)*2]; // number of columns of face is one less than number of vertices MULTIPLIED by two (triangles)

		for (int j=0; j<sideLength-1; j++) {
			// calculate surface normal of first triangle of unit square
			currFace = CrossProduct(j, yHeights[i][j], i, j, yHeights[i+1][j], i+1, j+1, yHeights[i][j+1], i);
			faceNorms[i][j*2] = new float[3];
			faceNorms[i][j*2][0] = currFace[0];
			faceNorms[i][j*2][1] = currFace[1];
			faceNorms[i][j*2][2] = currFace[2];

			// calculate surface normal of second triangle of unit square
			currFace = CrossProduct(j+1, yHeights[i][j+1], i, j, yHeights[i+1][j], i+1, j+1, yHeights[i+1][j+1], i+1);
			faceNorms[i][j*2+1] = new float[3];
			faceNorms[i][j*2+1][0] = currFace[0];
			faceNorms[i][j*2+1][1] = currFace[1];
			faceNorms[i][j*2+1][2] = currFace[2];
		}
	}
	
}

// returns cross product, used in CalculateSurfaceNormals
std::vector<float> Mesh::CrossProduct(int triA1, float triA2, int triA3, int triB1, float triB2, int triB3, int triC1, float triC2, int triC3) {
	std::vector<float> normal;
	float U[] = {triB1-triA1, triB2-triA2, triB3-triA3};
	float V[] = {triC1-triA1, triC2-triA2, triC3-triA3};

	normal.push_back((U[1]*V[2]) - (U[2]*V[1]));
	normal.push_back((U[2]*V[0]) - (U[0]*V[2]));
	normal.push_back((U[0]*V[1]) - (U[1]*V[0]));

	return normal;
}

// calculate vertex normals using previously calculated surface normals
void Mesh::CalculateVertexNormals() {
	vertNorms = new float**[sideLength];

	for (int i=0; i<sideLength; i++) {
		vertNorms[i] = new float*[sideLength];

		for (int j=0; j<sideLength; j++) {
			vertNorms[i][j] = new float[3];
			int shared = 0;
			float currVert[] = {0.0, 0.0, 0.0};

			// only loops through specific faces that share the vertex in question
			for (int m=i-1; m<i+1; m++) {
				if (m>=0 && m<sideLength-1) {
					for (int n=j*2-2+i-m; n<j*2+1+i-m; n++) {
						if (n>=0 && n<(sideLength-1)*2) {	// adds face normals info to temporary array
							currVert[0]+=faceNorms[m][n][0];
							currVert[1]+=faceNorms[m][n][1];
							currVert[2]+=faceNorms[m][n][2];
							shared++;
						}
					}
				}
			}

			currVert[0]/=(float)(shared);			// averaged face normals yield vertex normal
			currVert[1]/=(float)(shared);
			currVert[2]/=(float)(shared);

			// normalize vertex normal to unit vector
			float normalMagn = sqrt(pow(currVert[0],2) + pow(currVert[1],2) + pow(currVert[2],2));

			vertNorms[i][j][0] = currVert[0]/normalMagn;
			vertNorms[i][j][1] = currVert[1]/normalMagn;
			vertNorms[i][j][2] = currVert[2]/normalMagn;
		}
	}
}

// initialize 2D array of heights
void Mesh::InitMesh(int dimension) {
	sideLength = dimension;							// set terrain size to input size
	yHeights = new float*[sideLength];

	for (int i=0; i<sideLength; i++) {				// initialize 2D array of heights
		yHeights[i] = new float[sideLength];
		for (int j=0; j < sideLength; j++) {
			yHeights[i][j] = 0;						// initialize each height to 0 (flat terrain)
		}
	}

	GenerateHeights();								// generate peaks
	CalculateSurfaceNormals();						// calculate normals for lighting
	CalculateVertexNormals();
	getMinMaxHeights();

	xTeaMan = sideLength/2;							// initialize Teapot Man's position
	zTeaMan = sideLength/2;
}

// render terrain
void Mesh::DrawMesh(int wire, int strip, bool light) {
	// wire is {1, 2, 3}
	// wire 1 is terrain mode (filled shapes)
	// wire 2 is wireframe mode
	// wire 3 is both (draws filled shapes first then wireframe)
	// strip is {1, 2}
	// strip 1 is triangle strips
	// strip 2 is quad strips
	glCullFace(GL_BACK);

	// material info for terrain (grassy hills...kind of)
	float m_ambi[4] = {0.0, 0.05, 0.0, 1};
	float m_diff[4] = {0.1, 0.5, 0.1, 1};
	float m_spec[4] = {0.0, 0.1, 0.0, 1};
	float m_shin = 15;

	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambi);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT, GL_SHININESS, m_shin);

	// enable/disable lighting
	if (light) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}

	// draw filled terrain/wireframe
	if (wire==1 || wire==3) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 	// filled
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 	// wireframe
		glDisable(GL_LIGHTING);						// always draw black mesh
	}

	// draw mesh
	for (int i=0; i<sideLength-1; i++) {
		if (strip==1) {
			glBegin(GL_TRIANGLE_STRIP);				// render terrain as triangles
		} else {
			glBegin(GL_QUAD_STRIP);					// render terrain as quadrilaterals
		}

		for (int j=0; j<sideLength; j++) {
			if (wire==2) {							// wireframe
				glColor3f(0,0,0);
				glVertex3f(j, yHeights[i][j]+0.02, i);
				glVertex3f(j, yHeights[i+1][j]+0.02, i+1);
			} else {								// solid terrain
				// lighting off
				float gray = (yHeights[i][j]-minAltitude) / deltaAltitude;
				glColor3f(gray*0.2, 0.3+(gray*0.6), gray*0.2);

				// lighting on
				glNormal3f(vertNorms[i][j][0], vertNorms[i][j][1], vertNorms[i][j][2]);
				glVertex3f(j, yHeights[i][j], i);

				// lighting off
				gray = (yHeights[i+1][j]-minAltitude) / deltaAltitude;
				glColor3f(gray*0.2, 0.3+(gray*0.6), gray*0.2);

				// lighting on
				glNormal3f(vertNorms[i+1][j][0], vertNorms[i+1][j][1], vertNorms[i+1][j][2]);
				glVertex3f(j, yHeights[i+1][j], i+1);
			}
		}
		glEnd();
	}

	if (wire==3) {									// run DrawMesh again, but as wireframe mode
		DrawMesh(2, strip, light);
	}

	// always render Teapot Man solid
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPushMatrix();									// where Teapot Man is in relation to terrain, uses height information
		glTranslatef(xTeaMan, yHeights[(int)(ceil(zTeaMan))][(int)(ceil(xTeaMan))], zTeaMan);
		glRotatef(90, 0, 1, 0);
		DrawTeapotMan();
	glPopMatrix();
}

// render overview
void Mesh::DrawOverview() {
	glBegin(GL_POINTS);
	for (int i=0; i<sideLength; i++) {
		for (int j=0; j<sideLength; j++) {			// draws each vertex by pixel
			float gray = (yHeights[i][j]-minAltitude) / deltaAltitude;
			glColor3f(gray, gray, gray);
			glVertex2f(j, i);
		}
	}
	glEnd();
}

// function to actually change the heights surrounding a point
void Mesh::AlterAltitude(int direction, int xin, int zin) {
	float radius = sideLength/10.0;					// radius depends on terrain size
	float disp = sideLength/55.0;					// displacement depends of terrain size

	for (int i=0; i < sideLength; i++) {
		for (int j=0; j < sideLength; j++) {
			float pd = sqrt(pow(j-xin, 2) + pow(i-zin, 2)) * 2 / radius;

			if (pd <= 1.0) {
				if (direction == 1) {				// left click/build upwards
					yHeights[i][j] += disp/2 + cos(pd*3.14)*disp/2;
				} else {							// right click/dig downwards
					yHeights[i][j] -= disp/2 + cos(pd*3.14)*disp/2;
				}
			}
		}
	}
}

// updates min & max heights to show height gradient in colour
void Mesh::getMinMaxHeights() {
	maxAltitude = -25;
	minAltitude = 25;

	for (int i=0; i < sideLength; i++) {
		for (int j=0; j < sideLength; j++) {
			maxAltitude = std::max(maxAltitude, yHeights[i][j]);
			minAltitude = std::min(minAltitude, yHeights[i][j]);
		}
	}

	deltaAltitude = maxAltitude-minAltitude;		// difference in height between lowest and highest point
}

// renders Teapot Man
void Mesh::DrawTeapotMan() {
	//material info for Teapot Man (he's red)
	float m_ambi[4] = {0.05, 0.0, 0.0, 1};
	float m_diff[4] = {0.5, 0.1, 0.1, 1};
	float m_spec[4] = {0.9, 0.0, 0.0, 1};
	float m_shin = 25;

	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambi);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT, GL_SHININESS, m_shin);

	glPushMatrix();									// head
		glCullFace(GL_FRONT);
		glScalef(2, 2, 2);
		glTranslatef(0, 2.7, 0);
		glutSolidTeapot(1);
	glPopMatrix();
	glCullFace(GL_BACK);
	glPushMatrix();									// body
		glScalef(1, 2, 1);
		glTranslatef(0, 1, 0);
		glutSolidSphere(1, 12, 12);
	glPopMatrix();
	glPushMatrix();									// left leg
		glTranslatef(0, 0.4, 1);
		glutSolidSphere(1, 12, 12);
	glPopMatrix();
	glPushMatrix();									// right leg
		glTranslatef(0, 0.4, -1);
		glutSolidSphere(1, 12, 12);
	glPopMatrix();
	glPushMatrix();									// left arm
		glScalef(0.6, 1.2, 0.6);
		glTranslatef(0, 1.8, 1.5);
		glutSolidSphere(1, 12, 12);
	glPopMatrix();
	glPushMatrix();									// right arm
		glScalef(0.6, 1.2, 0.6);
		glTranslatef(0, 1.8, -1.5);
		glutSolidSphere(1, 12, 12);
	glPopMatrix();

}

// moves Teapot Man
void Mesh::MoveTeapotMan(float xin, float zin) {
	xTeaMan+=xin;
	zTeaMan+=zin;
	if (xTeaMan<0)
		xTeaMan = 0;
	if (xTeaMan>sideLength-1)
		xTeaMan = sideLength-1;
	if (zTeaMan<0)
		zTeaMan = 0;
	if (zTeaMan>sideLength-1)
		zTeaMan = sideLength-1;
}