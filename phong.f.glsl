#version 130

uniform mat4 perspective_mat, modelview_mat, object_mat;
uniform vec4 specular;
uniform float shininess;

const vec3 light_direction = vec3(-1, -1, 1);
const vec4 light_diffuse = vec4(0.7, 0.7, 0.7, 0.0);
const vec4 light_ambient = vec4(0.3, 0.3, 0.3, 1.0);
const vec4 light_specular = vec4(0.8, 0.8, 1.0, 1.0);

const vec4 spec=vec4(1.0, 1.0, 1.0, 0.0);
const float shine=10;

varying vec3 frag_position;
varying vec3 frag_color;
varying vec3 frag_normal;

void main(){

	vec3 modelview_light_dir=(modelview_mat*vec4(light_direction, 0)).xyz,
		normal=normalize(frag_normal),
		eye=normalize(frag_position),
		reflection=normalize(reflect(modelview_light_dir, normal));

	vec4 diffuse_factor=max(-dot(normal, modelview_light_dir), 0.0)*light_diffuse;
	vec4 diffuse_ambient_factor=diffuse_factor+light_ambient;
	vec4 specular_factor=pow(max(-dot(reflection, eye), 0.0), shine)*light_specular;

	gl_FragColor=specular_factor*specular+diffuse_ambient_factor*vec4(frag_color, 1);


}
