#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <vector>
#include <iostream>
#include <cmath>
#include "utils.h"
#include "matrices.h"
#include "sphere.h"
#define SCREENSZ 900

struct{

	struct{
		GLuint vertex_buffer, element_buffer; //vertex and element buffers to render the grid
		GLuint vertex_shader, fragment_shader, program; //shader program for the grid
		//grid shader internal vars:
		GLint position;
		GLint color;
		GLint perspective_mat;
		GLint modelview_mat;
	}grid;
	HomogMatrix p_matrix; //perspective matrix
	HomogMatrix m_matrix; //modelview matrix
	HomogMatrix o_matrix; //object matrix for sphere
	GLfloat shininess;
	GLfloat specular[4];

	struct{
		GLuint vertex_buffer, element_buffer; //vertex and element buffers to render the sphere
		GLuint vertex_shader, fragment_shader, program; //shader program for the spheres 
		//sphere shader internal vars:
		GLint position;
		GLint color;
		GLint perspective_mat;
		GLint modelview_mat;
		GLint object_mat;
		GLint specular, shininess;
		int nTr;

	}spheres;

	struct {
		GLfloat x, y, z; // viewer coords
		GLfloat theta, phi; //theta: angle about y axis(look left-right), phi: angle about x axis (look up-down)
	} viewer;

} g_resources;

bool create_program(std::string VSfile, std::string FSfile, GLuint& vertex_shader, GLuint& fragment_shader, GLuint& program){
	vertex_shader=make_shader(GL_VERTEX_SHADER, VSfile);
	if(0==vertex_shader){
		std::cout<<"Vertex Shader failed to compile"<<std::endl;
		return false;
	}
	fragment_shader=make_shader(GL_FRAGMENT_SHADER, FSfile);
	if(0==fragment_shader){
		std::cout<<"Fragment Shader failed to compile"<<std::endl;
		return false;
	}
	program=make_program(vertex_shader, fragment_shader);
	if(0==program){
		std::cout<<"Couldn't link shader program"<<std::endl;
		return false;
	}
	return true;
}

bool make_resources(){
	g_resources.shininess=1;
	g_resources.specular[0]=1.0f;
	g_resources.specular[1]=1.0f;
	g_resources.specular[2]=1.0f;
	g_resources.specular[3]=0.0f;

	//set viewer conditions
	g_resources.viewer.x=0;
	g_resources.viewer.y=10;
	g_resources.viewer.z=0;
	g_resources.viewer.theta=0;
	g_resources.viewer.phi=0;
	//generate grid
	std::vector<glPointStr> pointData;
	std::vector<GLushort> indices;
	int vertexCount=0;
	for(int i=-50; i<51; ++i){
		pointData.push_back(glPointStr(i*3, 0, -50*3, 1, 0, 0));
		indices.push_back(vertexCount++);
		pointData.push_back(glPointStr(i*3, 0, 50*3, 1, 0, 0));
		indices.push_back(vertexCount++);
	}
		
	for(int i=-50; i<51; ++i){
		pointData.push_back(glPointStr(-50*3, 0, i*3, 0, 0, 1));
		indices.push_back(vertexCount++);
		pointData.push_back(glPointStr(50*3, 0, i*3, 0, 0, 1));
		indices.push_back(vertexCount++);
	}
	//generate resources for grid
	g_resources.grid.vertex_buffer=make_buffer(GL_ARRAY_BUFFER, pointData.data(), sizeof(glPointStr)*pointData.size());
	g_resources.grid.element_buffer=make_buffer(GL_ELEMENT_ARRAY_BUFFER, indices.data(), sizeof(GLushort)*indices.size());
	if(!create_program("circle.v.glsl", "circle.f.glsl", g_resources.grid.vertex_shader, g_resources.grid.fragment_shader, 
				g_resources.grid.program)){
		return false;
	}

	//shader internal vars for grid shader
	g_resources.grid.position=glGetAttribLocation(g_resources.grid.program, "position");
	g_resources.grid.color=glGetAttribLocation(g_resources.grid.program, "color");
	g_resources.grid.perspective_mat=glGetUniformLocation(g_resources.grid.program, "perspective_mat");
	g_resources.grid.modelview_mat=glGetUniformLocation(g_resources.grid.program, "modelview_mat");
	//generate sphere and resources
	struct sphere ball=makeSphere( .7, .3, 0);
	g_resources.spheres.nTr=ball.idx.size();
	g_resources.spheres.vertex_buffer=make_buffer(GL_ARRAY_BUFFER, &(ball.points[0]), sizeof(glPointStr)*ball.points.size());
	g_resources.spheres.element_buffer=make_buffer(GL_ELEMENT_ARRAY_BUFFER, &(ball.idx[0]), sizeof(GLushort)*ball.idx.size());

	if(!create_program("phong.v.glsl", "phong.f.glsl", g_resources.spheres.vertex_shader, 
				g_resources.spheres.fragment_shader, g_resources.spheres.program)){
		return false;
	}
	//shader internal vars for spheres
	g_resources.spheres.position=glGetAttribLocation(g_resources.spheres.program, "position");
	g_resources.spheres.color=glGetAttribLocation(g_resources.spheres.program, "color");
	g_resources.spheres.perspective_mat=glGetUniformLocation(g_resources.spheres.program, "perspective_mat");
	g_resources.spheres.modelview_mat=glGetUniformLocation(g_resources.spheres.program, "modelview_mat");
	g_resources.spheres.object_mat=glGetUniformLocation(g_resources.spheres.program, "object_mat");
	g_resources.spheres.shininess=glGetUniformLocation(g_resources.spheres.program, "shininess");
	g_resources.spheres.specular=glGetUniformLocation(g_resources.spheres.program, "specular");

	g_resources.p_matrix=g_resources.p_matrix.view_frustum(deg2rad(30), 1.0, 0.5, 800);
	//view_frustum(g_resources.p_matrix, deg2rad(30), 1.0, 0.5, 800);
	g_resources.m_matrix=g_resources.m_matrix.translate(-g_resources.viewer.x, -g_resources.viewer.y, -g_resources.viewer.z);
	//translate(g_resources.m_matrix, -g_resources.viewer.x, -g_resources.viewer.y, -g_resources.viewer.z);
	//translate(g_resources.o_matrix, 0, 0, 0);
	g_resources.o_matrix=HomogMatrix().scale(4, 4, 4).translate(10, 0, 40);

	return true;

}

void render(){


	//translate(g_resources.o_matrix, 40, 0, 40);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(g_resources.grid.program);
	glUniformMatrix4fv(g_resources.grid.perspective_mat, 1, GL_FALSE, g_resources.p_matrix.getData());
	glUniformMatrix4fv(g_resources.grid.modelview_mat, 1, GL_FALSE, g_resources.m_matrix.getData());
	glBindBuffer(GL_ARRAY_BUFFER, g_resources.grid.vertex_buffer);
	glEnableVertexAttribArray(g_resources.grid.position);
	glEnableVertexAttribArray(g_resources.grid.color);
	glVertexAttribPointer(g_resources.grid.position, 4, GL_FLOAT, GL_FALSE, sizeof(glPointStr), (void*) 0);
	glVertexAttribPointer(g_resources.grid.color, 4, GL_FLOAT, GL_FALSE, sizeof(glPointStr), (void*) (offsetof(glPointStr, r)));

	glLineWidth(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.grid.element_buffer);
	glDrawElements(GL_LINES, 4*255, GL_UNSIGNED_SHORT, (void*) 0);

	glDisableVertexAttribArray(g_resources.grid.position);
	glDisableVertexAttribArray(g_resources.grid.color);

	//draw sphere
	glUseProgram(g_resources.spheres.program);
	glUniformMatrix4fv(g_resources.spheres.perspective_mat, 1, GL_FALSE, g_resources.p_matrix.getData());
	glUniformMatrix4fv(g_resources.spheres.modelview_mat, 1, GL_FALSE, g_resources.m_matrix.getData());
	glUniformMatrix4fv(g_resources.spheres.object_mat, 1, GL_FALSE, g_resources.o_matrix.getData());
	glUniform1f(g_resources.spheres.shininess, g_resources.shininess);
	glUniform4fv(g_resources.spheres.specular, 1, g_resources.specular);
	glBindBuffer(GL_ARRAY_BUFFER, g_resources.spheres.vertex_buffer);
	glEnableVertexAttribArray(g_resources.spheres.position);
	glEnableVertexAttribArray(g_resources.spheres.color);
	glVertexAttribPointer(g_resources.spheres.position, 4, GL_FLOAT, GL_FALSE, sizeof(glPointStr), (void*) 0);
	glVertexAttribPointer(g_resources.spheres.color, 4, GL_FLOAT, GL_FALSE, sizeof(glPointStr), (void*) (offsetof(glPointStr, r)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.spheres.element_buffer);
	glDrawElements(GL_TRIANGLES, g_resources.spheres.nTr, GL_UNSIGNED_SHORT, (void*) 0);
	glDisableVertexAttribArray(g_resources.spheres.position);
	glDisableVertexAttribArray(g_resources.spheres.color);

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

	g_resources.m_matrix=((HomogMatrix().translate(-g_resources.viewer.x, -g_resources.viewer.y, -g_resources.viewer.z)).rotate_y(deg2rad(g_resources.viewer.theta))).rotate_x(deg2rad(g_resources.viewer.phi));
	
}
int main(int argc, char** argv){

	if(!initSDL(SCREENSZ)){
		std::cout<<"Failed to init SDL"<<std::endl;
		return 1;
	}
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
		render();
	}

	SDL_Quit();

	return 0;
}

