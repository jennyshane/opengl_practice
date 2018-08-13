#version 130


uniform mat4 perspective_mat;
uniform mat4 modelview_mat;
uniform mat4 object_mat;
attribute vec4 position;
attribute vec4 color;

varying vec4 fcolor;

void main(){

	gl_Position=perspective_mat*modelview_mat*object_mat*position;
	fcolor=color;
}
