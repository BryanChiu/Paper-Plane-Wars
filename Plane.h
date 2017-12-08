#ifndef __PLANE_H__
#define __PLANE_H__

#include <vector>

class Plane {
	public:
		Plane();

		void InitPlane(float gravin, float scalin, float xin, float yin, float zin, float xang, float yang);
		void DrawPlane(); // draws plane (with scaling)
		void MovePlane(); // moves plane according to velocity
		// void AimPlane(int xPitch, int yYaw);
		void SetPitch(int timerIn);
		void SetYaw(int timerIn);
		void SetPower(int timerIn);
		void LaunchPlane();

		std::vector<float> getCoords(); // return coords to translate plane's matrix
		std::vector<float> getOrient(); // return orientation angles to rotate plane's matrix
		float getPower();
		
		bool inFlight;
		
	private:
		float gravity;
		float airFriction;
		float scale;

		std::vector<float> pos;
		std::vector<float> vel;
		
		float pitch;
		float yaw;
		float power;

};

#endif