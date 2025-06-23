#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include "omelette_style.h"

class Mesh {
public:
	dynamic_array<Vertex> vertices;
	dynamic_array<GLuint> indices;

	VAO vao;

	Mesh(dynamic_array<Vertex>& vertices, dynamic_array<GLuint>& indices);

	void Draw(Shader& shader, Camera& camera);

	void render();
};
#endif