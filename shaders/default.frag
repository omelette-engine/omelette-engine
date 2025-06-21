#version 330 core
out vec4 FragColour;

in vec3 colour;
in vec3 Normal;
in vec3 current_position;

uniform vec3 light_position;
uniform vec4 light_colour;
uniform vec3 lit_colour;
uniform vec3 shadow_colour;

void main() {
   vec3 normal = normalize(Normal);
   vec3 light_direction = normalize(light_position - current_position);
   float diffuse = max(dot(normal, light_direction), 0.0f);
   
   vec3 final_colour = (diffuse > 0.0) ? lit_colour : shadow_colour;
   
   FragColour = vec4(final_colour, 1.0f);
}