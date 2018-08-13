#include <GL/glu.h>
#include <vector>
#include <cmath>
#include "sphere.h"
#include "utils.h"
#include "matrices.h"


const struct sphere makeSphere(GLfloat r, GLfloat g, GLfloat b){

	std::vector<glPointStr> points;
	std::vector<GLushort> idx;

	points.push_back(glPointStr(0.0f, 1.0f, 0.0f, r, g, b));

	for(float i=10; i<180; i+=10){
		for(float j=0; j<360; j+=10){
			points.push_back(glPointStr(cos(pi*j/180)*sin(pi*i/180), cos(pi*i/180), 
						sin(pi*j/180)*sin(pi*i/180), r, g, b));
		}
	}

	points.push_back(glPointStr(0.0f, -1.0f, 0.0f, r, g, b));

	int nThetas=360.0/10;
	int nPhis=180.0/10;

	int offset=1;
	for(int i=0; i<nThetas-1; i++){
		idx.push_back(0);
		idx.push_back(i+offset);
		idx.push_back(i+1+offset);
	}
	idx.push_back(0);
	idx.push_back(nThetas-1+offset);
	idx.push_back(offset);

	int tOffset=1;
	int bOffset=1+nThetas;

	for(int j=1; j<nPhis-1; ++j){
		for(int i=0; i<nThetas-1; ++i){
			idx.push_back(i+tOffset);
			idx.push_back(i+bOffset);
			idx.push_back(i+1+tOffset);

			idx.push_back(i+bOffset);
			idx.push_back(i+1+bOffset);
			idx.push_back(i+1+tOffset);
		}
		idx.push_back(nThetas-1+tOffset);
		idx.push_back(nThetas-1+bOffset);
		idx.push_back(tOffset);

		idx.push_back(nThetas-1+bOffset);
		idx.push_back(bOffset);
		idx.push_back(tOffset);

		tOffset+=nThetas;
		bOffset+=nThetas;
	}

	for(int i=0; i<nThetas-1; ++i){
		idx.push_back(i+1+tOffset);
		idx.push_back(i+tOffset);
		idx.push_back(bOffset);
	}

	idx.push_back(tOffset);
	idx.push_back(nThetas-1+tOffset);
	idx.push_back(bOffset);

	//struct sphere ball(points, idx, colors);
	struct sphere ball(points, idx);

	return ball;
}
