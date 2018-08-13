#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <GL/glu.h>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include "matrices.h"

struct glFPointStr{ //struct for points in a flat (non-shaded) render
	GLfloat x, y, z, w;
	GLfloat r, g, b, a;
	glFPointStr(float X, float Y, float R, float G, float B): x(X), y(Y), r(R), g(G), b(B)
	{
		z=0.0f;
		w=1.0f;
		a=1.0f;
	}

	glFPointStr(float X, float Y, float Z, float R, float G, float B): x(X), y(Y), z(Z), r(R), g(G), b(B)
	{
		w=1.0f;
		a=1.0f;
	}

	glFPointStr(){ //default constructor
		x=0.0f;
		y=0.0f;
		z=0.0f;
		w=1.0f;
		r=0.0f;
		g=0.0f;
		b=0.0f;
		a=1.0f;
	}

};

struct glPhPointStr{
	GLfloat position[4];
	GLfloat color[4];
	GLfloat normal[4];
	GLfloat shininess;
	GLfloat specular[4];

	glPhPointStr(GLfloat* pos, GLfloat* col, GLfloat* norm, GLfloat shine, GLfloat* spec){
		std::copy(pos, pos+4, position);
		std::copy(col, col+4, color);
		std::copy(norm, norm+4, normal);
		shininess=shine;
		std::copy(spec, spec+4, specular);
	}
	glPhPointStr(GLfloat* pos, GLfloat* col, GLfloat* norm){
		std::copy(pos, pos+4, position);
		std::copy(col, col+4, color);
		std::copy(norm, norm+4, normal);
		shininess=0.0; 
		specular[0]=0;
		specular[1]=0;
		specular[2]=0;
		specular[3]=0;
	}
	glPhPointStr(){
		GLfloat tmp[4]={0.0f, 0.0f, 0.0f, 0.0f};
		std::copy(tmp, tmp+4, position);
		std::copy(tmp, tmp+4, color);
		std::copy(tmp, tmp+4, normal);
		shininess=0.0; 
		std::copy(tmp, tmp+4, specular);
	}

};

class glObject{ // base class for glObjects
	protected:
		GLuint vertex_buffer;
		GLuint element_buffer;
		std::vector<GLushort> index_data;
		std::vector<std::function<void(void)> > render_commands; //commands to execute in the render function
		std::vector<std::pair<std::string, int> > data_offsets; //identifier passed by user for each chunk 
				//of vertex data + offset in vec
		static HomogMatrix p_matrix;
		static HomogMatrix m_matrix;
		HomogMatrix o_matrix;

	public: 
		virtual void render()=0;
		void setRenderRule(std::string identifier, GLenum mode, std::vector<GLushort> indices);
		void setOMat(HomogMatrix mat);
		static void setMMat(HomogMatrix mat);
		static void setPMat(HomogMatrix mat);
};

class glPhObject : public glObject{ //phong shaded object
	private:
		static GLuint program;
		static bool isInit;
		std::vector<glPhPointStr> vertex_data;
		static GLint position, color;
		static GLint object_mat, modelview_mat, perspective_mat;

	public:
		glPhObject(); //init program here
		void addData(std::string identifier, std::vector<glPhPointStr> data);
		void initGL();

};

class glFObject : public glObject{ //flat shaded object
	private:
		static GLuint program;
		static bool isInit;
		std::vector<glFPointStr> vertex_data;
		static GLint position, color;
		static GLint object_mat, modelview_mat, perspective_mat;

	public:
		glFObject(); //init program here
		void addData(std::string identifier, std::vector<glFPointStr> data);
		void initGL();
		void render();

};

#endif
