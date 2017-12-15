#ifndef __HUMAN_H__
#define __HUMAN_H__

#include <vector>

class Human {
	public:
		Human();

		void InitHuman(int id, float xin, float yin, float zin);
		void DrawHuman(); // draws human
		void DodgePlane(std::vector<float> planePos);

		int getID();
		std::vector<float> getCoords(); // return coords to translate plane's matrix
		std::vector<float> getOrient(); // return orientation angles to rotate plane's matrix
		
	private:
		int ID;
		std::vector<float> pos;
		std::vector<float> rot;
		std::vector<float> boundMin, boundMax;

		
};

#endif