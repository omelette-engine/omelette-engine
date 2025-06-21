#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 acolour;
layout (location = 2) in vec3 aNormal;

out vec3 colour;
out vec3 Normal;
out vec3 current_position;

uniform mat4 camera_matrix;
uniform mat4 model;

void main(){
   current_position = vec3(model * vec4(aPos, 1.0f));
   gl_Position = camera_matrix * vec4(current_position, 1.0);
   colour = acolour;
   Normal = mat3(transpose(inverse(model))) * aNormal;
}