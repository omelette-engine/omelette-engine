#version 330 core

layout (location = 0) in vec3 position_attribute;

uniform mat4 model;
uniform mat4 camera_matrix;

void main(){
	gl_Position = camera_matrix * model * vec4(position_attribute, 1.0f);
}