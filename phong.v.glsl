#version 130

uniform float timer;
uniform mat4 perspective_mat, modelview_mat, object_mat;

attribute vec4 position;
attribute vec4 color;

varying vec3 frag_position;
varying vec3 frag_color;
varying vec3 frag_normal;

void main(){

	vec4 eye_pos=modelview_mat*object_mat*position;
	gl_Position=perspective_mat*eye_pos;
	frag_position=eye_pos.xyz;
	frag_normal=(modelview_mat*vec4(position.xyz, 0)).xyz;
	frag_color=color.xyz;

}
