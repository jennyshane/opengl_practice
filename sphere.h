#include <vector>
#include <GL/glu.h>
#include "utils.h"

struct sphere{
	const std::vector<glPointStr> points;
	const std::vector<GLushort> idx;
	//const std::vector<GLfloat> colors;
	GLfloat cx, cy, cz;

	/*
	sphere(std::vector<GLfloat> pts, std::vector<GLushort> ids, std::vector<GLfloat> cols) 
		: points(pts), idx(ids), colors(cols){}
		*/
	sphere(std::vector<glPointStr> pts, std::vector<GLushort> ids) : points(pts), idx(ids){}
};


const struct sphere makeSphere(GLfloat r, GLfloat g, GLfloat b);
