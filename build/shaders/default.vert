#version 330 core
layout (location = 0) in vec3 position_attribute;
layout (location = 1) in vec3 normal_attribute;
layout (location = 2) in vec3 colour_attribute;

out vec3 current_position;
out vec3 vertex_normal;
out vec3 colour;

uniform mat4 camera_matrix;
uniform mat4 model;

void main(){
   current_position = vec3(model * vec4(position_attribute, 1.0f));
   vertex_normal = mat3(model) * normal_attribute;
   colour = colour_attribute;
   gl_Position = camera_matrix * vec4(current_position, 1.0);
}