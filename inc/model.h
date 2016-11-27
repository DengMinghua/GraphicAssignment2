#ifndef _MODEL_H_
#define _MODEL_H_

#include <GL/glut.h>
#include <GL/glu.h>
#include "objshape.h"
#include <string>
using namespace std;

class Model{
	public:
		void Draw();
		void DrawLines();
		void DrawPoints();
		void Rebuild();
	public:
		/*
		std::vector<objTriangle> ts;
		std::vector<objRect> rs;
		*/
		string name;
		std::vector<objLine> ls;
		std::vector<objPoly> ps;
		int triangleNum;
		int rectNum;
	private:
		void DrawObjPoint(const objPoint &p);
};

#endif
