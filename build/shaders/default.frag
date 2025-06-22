#version 330 core
out vec4 fragment_colour;

in vec3 current_position;
in vec3 vertex_normal;
in vec3 colour;

uniform vec3 light_position;
uniform vec4 light_colour;
uniform vec3 lit_colour;
uniform vec3 shadow_colour;

vec4 point_light(){
   vec3 light_vector = light_position - current_position;
   float distance = length(light_vector);
   float a = 3.0;
   float b = 0.7;
   float intensity = 1.0f / (a * distance * distance + b * distance + 1.0f);

   vec3 normal = normalize(vertex_normal);
   vec3 light_direction = normalize(light_vector);
   float diffuse = max(dot(normal, light_direction), 0.0f) * intensity;
   
   vec3 final_colour = (diffuse > 0.3) ? lit_colour : shadow_colour;

   return vec4(final_colour, 1.0f);
   //return vec4(intensity, intensity, intensity, 1.0f);
}

vec4 sun_light(){
   vec3 normal = normalize(vertex_normal);
   vec3 light_direction = normalize(vec3(1.0f, 1.0f, 0.0f));
   float diffuse = max(dot(normal, light_direction), 0.0f);
   
   vec3 final_colour = (diffuse > 0.0) ? lit_colour : shadow_colour;
   
   return vec4(final_colour, 1.0f);
}

vec4 spot_light(){
   // controls how big the area that is lit up is
   float outer_cone = 0.90f;

   // diffuse lighting
   vec3 normal = normalize(vertex_normal);
   vec3 light_direction = normalize(light_position - current_position);
   float diffuse = max(dot(normal, light_direction), 0.0f);

   // calculates if we're inside the light cone - DIRECT CUTOFF!
   float angle = dot(vec3(0.0f, -1.0f, 0.0f), -light_direction);
   float intensity = (angle > outer_cone) ? 1.0f : 0.0f;

   // combine diffuse and spotlight intensity for binary decision
   float final_light = diffuse * intensity;
   vec3 final_colour = (final_light > 0.0) ? lit_colour : shadow_colour;

   return vec4(final_colour, 1.0f);
}

void main() {
   vec3 normal = normalize(vertex_normal);
   vec3 light_direction = normalize(light_position - current_position);
   float diffuse = max(dot(normal, light_direction), 0.0f);
   
   vec3 final_colour = (diffuse > 0.0) ? lit_colour : shadow_colour;
   
   fragment_colour = point_light();
}