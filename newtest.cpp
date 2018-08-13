#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include "utils.h"
#include "matrices.h"
#include "glObject.h"



struct{

	glFObject box;
	glFObject grid;
	
	struct {
		GLfloat x, y, z; // viewer coords
		GLfloat theta, phi; //theta: angle about y axis(look left-right), phi: angle about x axis (look up-down)
	} viewer;

} g_resources;


bool make_resources(){
	//set viewer conditions
	g_resources.viewer.x=0;
	g_resources.viewer.y=10;
	g_resources.viewer.z=0;
	g_resources.viewer.theta=0;
	g_resources.viewer.phi=0;
	//generate grid
	std::vector<glFPointStr> pointData;
	std::vector<GLushort> indices;
	int vertexCount=0;
	for(int i=-50; i<51; ++i){
		pointData.push_back(glFPointStr(i*3, 0, -50*3, 1, 0, 0));
		indices.push_back(vertexCount++);
		pointData.push_back(glFPointStr(i*3, 0, 50*3, 1, 0, 0));
		indices.push_back(vertexCount++);
	}
		
	for(int i=-50; i<51; ++i){
		pointData.push_back(glFPointStr(-50*3, 0, i*3, 0, 0, 1));
		indices.push_back(vertexCount++);
		pointData.push_back(glFPointStr(50*3, 0, i*3, 0, 0, 1));
		indices.push_back(vertexCount++);
	}
	g_resources.grid.addData(std::string("grid"), pointData);
	g_resources.grid.setRenderRule(std::string("grid"), GL_LINES, indices);
	
	std::vector<glFPointStr> cubeData;
	cubeData.push_back(glFPointStr(0, 0, 0, 0, 1, 0));
	cubeData.push_back(glFPointStr(0, 0, 6, 0, 1, 0));
	cubeData.push_back(glFPointStr(0, 6, 0, 0, 1, 0));
	cubeData.push_back(glFPointStr(0, 6, 6, 0, 1, 0));
	cubeData.push_back(glFPointStr(6, 0, 0, 0, 1, 0));
	cubeData.push_back(glFPointStr(6, 0, 6, 0, 1, 0));
	cubeData.push_back(glFPointStr(6, 6, 0, 0, 1, 0));
	cubeData.push_back(glFPointStr(6, 6, 6, 0, 1, 0));
	std::vector<GLushort> cubeidx={2, 0, 4, 4, 6, 2, 
					3, 1, 0, 0, 2, 3,
					6, 4, 5, 5, 7, 6,
					0, 1, 5, 5, 4, 0,
					3, 2, 6, 6, 7, 3,
					7, 5, 1, 1, 3, 7};

	g_resources.box.addData(std::string("cube"), cubeData);
	g_resources.box.setRenderRule(std::string("cube"), GL_TRIANGLES, cubeidx);
	for(auto &i: cubeData){
		i.r=1;
		i.b=1;
	}
	std::vector<GLushort> lines={0, 1, 0, 2, 
		0, 4, 2, 3, 
		1, 3, 7, 3, 
		7, 6, 7, 5, 
		5, 1, 5, 4, 
		6, 2, 6, 4};


	g_resources.box.addData(std::string("outline"), cubeData);
	g_resources.box.setRenderRule(std::string("outline"), GL_LINES, lines);

	glObject::setPMat(HomogMatrix().view_frustum(deg2rad(30), 1.0, 0.5, 800));
	//HomogMatrix p_matrix=g_resources.p_matrix.view_frustum(deg2rad(30), 1.0, 0.5, 800);
	//view_frustum(g_resources.p_matrix, deg2rad(30), 1.0, 0.5, 800);
	glObject::setMMat(HomogMatrix().translate(-g_resources.viewer.x, -g_resources.viewer.y, -g_resources.viewer.z));
	//translate(g_resources.m_matrix, -g_resources.viewer.x, -g_resources.viewer.y, -g_resources.viewer.z);
	//translate(g_resources.o_matrix, 0, 0, 0);
	g_resources.grid.setOMat(HomogMatrix().translate(0, 0, 0));
	g_resources.box.setOMat(HomogMatrix().translate(10, 0, 10));

	g_resources.grid.initGL();
	g_resources.box.initGL();
	return true;
}


void renderStuff(){


	//translate(g_resources.o_matrix, 40, 0, 40);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLineWidth(2);
	/*
	for(int i=0; i<g_resources.objects.size(); ++i){
		g_resources.objects[i].render();
	}
	*/
	g_resources.grid.render();
	g_resources.box.render();

	SDL_GL_SwapBuffers();

}


void handle_key(char key){
	GLfloat right[2]={cos(deg2rad(g_resources.viewer.theta)), sin(deg2rad(g_resources.viewer.theta))}; //x, z
	GLfloat forward[2]={-sin(deg2rad(g_resources.viewer.theta)), cos(deg2rad(g_resources.viewer.theta))};
	
	if('w'==key){
		g_resources.viewer.z+=5*forward[1]; //move forward
		g_resources.viewer.x+=5*forward[0]; 
	}else if('s'==key){
		g_resources.viewer.z-=5*forward[1]; //move back 
		g_resources.viewer.x-=5*forward[0]; 
	}else if('a'==key){
		g_resources.viewer.x-=5*right[0]; //move left 
		g_resources.viewer.z-=5*right[1]; 
	}else if('d'==key){
		g_resources.viewer.x+=5*right[0]; //move right 
		g_resources.viewer.z+=5*right[1]; 
	}else if('q'==key){
		g_resources.viewer.y-=5; //move up
	}else if('e'==key){
		g_resources.viewer.y+=5; //move down
	}else if('h'==key){
		g_resources.viewer.theta+=10; //turn left
	}else if('j'==key){
		g_resources.viewer.theta-=10; //turn right
	}else if('k'==key){
		g_resources.viewer.phi-=10; //look down 
	}else if('l'==key){
		g_resources.viewer.phi+=10; //look up
	}
	//g_resources.m_matrix=((HomogMatrix().rotate_y(deg2rad(g_resources.viewer.theta))).rotate_x(deg2rad(g_resources.viewer.phi))).translate(-g_resources.viewer.x, -g_resources.viewer.y, -g_resources.viewer.z);

	glObject::setMMat(((HomogMatrix().translate(-g_resources.viewer.x, -g_resources.viewer.y, -g_resources.viewer.z)).rotate_y(deg2rad(g_resources.viewer.theta))).rotate_x(deg2rad(g_resources.viewer.phi)));
	
}
int main(int argc, char** argv){

	/*
	if(!initSDL(SCREENSZ)){
		std::cout<<"Failed to init SDL"<<std::endl;
		return 1;
	}
	*/
	SDL_EnableUNICODE(1);
	SDL_Event event;
	bool quit=false;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	if(!make_resources()){
		std::cout<<"Couldn't make resources"<<std::endl;
		return 1;
	}
	while(false==quit){
		while(SDL_PollEvent(&event)){
			if(SDL_QUIT==event.type){
				quit=true;
			}else if(SDL_KEYDOWN==event.type){
				handle_key(event.key.keysym.unicode);
			}
		}
		renderStuff();
	}

	SDL_Quit();

	return 0;
}

