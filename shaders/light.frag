#version 330 core

out vec3 frag_colour;
uniform vec3 light_colour;


void main(){
    frag_colour = light_colour;
}