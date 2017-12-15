#ifndef __PLANE_H__
#define __PLANE_H__

#include <vector>

class Plane {
	public:
		Plane();

		void InitPlane(int id, char *file, float gravin, float xin, float yin, float zin, float xang, float yang);
		void DrawPlane(bool active); // active changes material
		void ExhibitPlane(int timerIn);
		void MovePlane(); // moves plane according to velocity
		void SetPitch(int timerIn);
		void SetYaw(int timerIn);
		void SetPower(int timerIn);
		void LaunchPlane();
		void BlowPlane(float xin, float yin);
		void Collision();

		int getID();
		std::vector<float> getCoords(); // return coords to translate plane's matrix
		std::vector<float> getOrient(); // return orientation angles to rotate plane's matrix
		float getPower();

		std::vector< std::vector<float> > getBoundFaceNorms();
		std::vector<float> getBoundFaceDists();
		
		bool inFlight;
		
	private:
		bool LoadOBJ(const char * path, std::vector<float> &out_vertices, std::vector<float> &out_uvs, std::vector<float> &out_normals);

		int ID;
		float gravity, airFriction; // 
		float pitch, yaw, roll, power;
		std::vector<float> pos;
		std::vector<float> vel;
		std::vector<float> vertices, uvs, normals;
		std::vector<float> boundMin, boundMax;
		std::vector< std::vector<float> > faceNorms;

		// struct for materials
		typedef struct materialStruct {
			float ambient[4];
			float diffuse[4];
			float specular[4];
			float shininess;
		} materialStruct;

		materialStruct materialMain;
		materialStruct materialSecondary;
		
};

#endif