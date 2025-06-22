#include "Mesh.h"
#include "Camera.h"
#include "shaderClass.h"
#include "no_abbreviations.h"

Mesh::Mesh(dynamic_array<Vertex>& vertices, dynamic_array<GLuint>& indices)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;

	vao.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);
	// Links VBO attributes such as coordinates and colors to VAO
    vao.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);                    // position
    vao.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // normals  
    vao.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // colours
	// Unbind all to prevent accidentally modifying them
	vao.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera){
    shader.Activate();
    vao.Bind();
    glUniform3f(glGetUniformLocation(shader.ID, "camera_position"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "camera_matrix");

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}