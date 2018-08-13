#include <GL/glew.h>
#include <GL/glu.h>
#include <cassert>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include <iostream>
#include "matrices.h"
#include "utils.h"
#include "glObject.h"

#define SCREENSZ 900

//~~~~~~~~~~~~~~glObject~~~~~~~~~~~~~~~~~~~~~~~

HomogMatrix glObject::p_matrix;
HomogMatrix glObject::m_matrix;

void glObject::setRenderRule(std::string identifier, GLenum mode, std::vector<GLushort> indices){
	int numelem=indices.size();
	int offset=index_data.size();
	render_commands.push_back([mode, numelem, offset](){glDrawElements(mode, numelem, GL_UNSIGNED_SHORT, 
				(void*) (offset*sizeof(GLushort)));}); //generate lambda function for this render command
	int offs=-1;
	for(auto i: data_offsets){
		if(i.first==identifier){
			offs=i.second;
			//std::cout<<offs<<std::endl;
		}
	}
	assert(offs!=-1);//identifier does not match
	for(auto &i: indices){
		i=i+offs;
	}
	index_data.insert(index_data.end(), indices.begin(), indices.end()); //add indices to list for element buffer
}

void glObject::setOMat(HomogMatrix mat){
	o_matrix.copy(mat);

}

void glObject::setMMat(HomogMatrix mat){
	m_matrix.copy(mat);
}


void glObject::setPMat(HomogMatrix mat){
	p_matrix.copy(mat);
}



//~~~~~~~~~~~~Flat Shaded glObject~~~~~~~~~~

bool glFObject::isInit=false;
GLuint glFObject::program;
GLint glFObject::color;
GLint glFObject::position;
GLint glFObject::perspective_mat;
GLint glFObject::modelview_mat;
GLint glFObject::object_mat;

glFObject::glFObject(){
	if(!isInit){
		initSDL(SCREENSZ);
		GLuint v_shader, f_shader;
		v_shader=make_shader(GL_VERTEX_SHADER, "circle.v.glsl");
		std::cout<<"!"<<std::endl;
		assert(v_shader!=0);
		f_shader=make_shader(GL_FRAGMENT_SHADER, "circle.f.glsl");
		assert(v_shader!=0);
		program=make_program(v_shader, f_shader);
		assert(program!=0);
		position=glGetAttribLocation(program, "position");
		color=glGetAttribLocation(program, "color");
		object_mat=glGetUniformLocation(program, "object_mat");
		modelview_mat=glGetUniformLocation(program, "modelview_mat");
		perspective_mat=glGetUniformLocation(program, "perspective_mat");
		isInit=true;
	}
}

void glFObject::addData(std::string identifier, std::vector<glFPointStr> data){
	for(int i=0; i<data_offsets.size(); ++i){
		if(identifier==data_offsets[i].first){
			std::cout<<"Error: data "<<identifier<<" already initialized"<<std::endl;
			return;
		}
	}

	data_offsets.push_back(std::pair<std::string, int>(identifier, vertex_data.size()));
	vertex_data.insert(vertex_data.end(), data.begin(), data.end());
}
void glFObject::initGL(){
	vertex_buffer=make_buffer(GL_ARRAY_BUFFER, vertex_data.data(), sizeof(glFPointStr)*vertex_data.size());
	element_buffer=make_buffer(GL_ELEMENT_ARRAY_BUFFER, index_data.data(), sizeof(GLushort)*index_data.size());
}

void glFObject::render(){
	glUseProgram(program);
	glUniformMatrix4fv(perspective_mat, 1, GL_FALSE, p_matrix.getData());
	glUniformMatrix4fv(modelview_mat, 1, GL_FALSE, m_matrix.getData());
	glUniformMatrix4fv(object_mat, 1, GL_FALSE, o_matrix.getData());

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(color);
	glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, sizeof(glFPointStr), (void*) 0);
	glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, sizeof(glFPointStr), (void*) (offsetof(glFPointStr, r)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	for(auto i:render_commands){
		i();
	}

	glDisableVertexAttribArray(position);
	glDisableVertexAttribArray(color);

}
