// #include <cmath>
#include <cstddef>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

// custom headers!
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

const unsigned int window_width = 800;
const unsigned int window_height = 800;

GLfloat vertices[] = {
    // Bottom face (y = 0)
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     0.0f, -1.0f, 0.0f, // 0
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     0.0f, -1.0f, 0.0f, // 1
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     0.0f, -1.0f, 0.0f, // 2
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     0.0f, -1.0f, 0.0f, // 3

    // Apex vertex (for all triangular faces)
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,     0.0f,  1.0f, 0.0f, // 4 (this normal will be overridden per face)

    // Left face vertices (reuse base vertices but with correct normals)
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,    -0.707f, 0.5f,  0.5f, // 5
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,    -0.707f, 0.5f, -0.5f, // 6
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,    -0.707f, 0.707f, 0.0f, // 7

    // Back face vertices
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.5f, -0.707f, // 8
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.5f, -0.707f, // 9
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,     0.0f, 0.707f, -0.707f, // 10

    // Right face vertices  
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     0.707f, 0.5f, -0.5f, // 11
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     0.707f, 0.5f,  0.5f, // 12
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,     0.707f, 0.707f, 0.0f, // 13

    // Front face vertices
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.5f,  0.707f, // 14
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.5f,  0.707f, // 15
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,     0.0f, 0.707f, 0.707f  // 16
};

GLuint indices[] = {
    // Bottom face (two triangles)
    0, 1, 2,
    0, 2, 3,
    
    // Side faces (each is one triangle)
    5, 7, 6,   // Left face
    8, 10, 9,  // Back face  
    11, 13, 12, // Right face
    14, 16, 15  // Front face
};

GLfloat lightVertices[] = { //     COORDINATES     //
	-0.1f, -0.1f,  0.1f,
	-0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f,  0.1f,
	-0.1f,  0.1f,  0.1f,
	-0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f,  0.1f
};

GLuint lightIndices[] = {
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


int main(){
    // initialise glfw
    glfwInit();


    // tell glfw what opengl version we're using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a new window with some params, and throw an error if you can't for some reason
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "omelette engine", NULL, NULL);
    if(window == NULL){
        std::cout << "failed to create glfw window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // even though we told the computer to make a window, we still have to tell it to _show_ the window
     glfwMakeContextCurrent(window);
     gladLoadGL();
     

     glViewport(0, 0, window_width, window_height);

     Shader shaderProgram("shaders/default.vert", "shaders/default.frag");

     VAO VAO1;
     VAO1.Bind();

     VBO VBO1(vertices, sizeof(vertices));
     EBO EBO1(indices, sizeof(indices));

     VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 9 * sizeof(float), (void*)0);
     VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 9 * sizeof(float), (void*)(3 * sizeof(float)));
     VAO1.LinkAttrib(VBO1, 2, 3, GL_FLOAT, 9 * sizeof(float), (void*)(6 * sizeof(float)));
     VAO1.Unbind();
     VBO1.Unbind();
     EBO1.Unbind();

     Shader light_shader("shaders/light.vert", "shaders/light.frag");

     VAO light_vao;
     light_vao.Bind();

     VBO light_vbo(lightVertices, sizeof(lightVertices));
     EBO light_ebo(lightIndices, sizeof(lightIndices));

     light_vao.LinkAttrib(light_vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

     light_vao.Unbind();
     light_vbo.Unbind();
     light_ebo.Unbind();

     glm::vec4 light_colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


     glm::vec3 light_position = glm::vec3(0.5f, 0.5f, 0.5f);
     glm::mat4 light_model = glm::mat4(1.0f);
     light_model = glm::translate(light_model, light_position);

     glm::vec3 pyramid_position = glm::vec3(0.0f, 0.0f, 0.0f);
     glm::mat4 pyramid_model = glm::mat4(1.0f);
     pyramid_model = glm::translate(pyramid_model, pyramid_position);

     light_shader.Activate();
     glUniformMatrix4fv(glGetUniformLocation(light_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(light_model));
     glUniform4f(glGetUniformLocation(light_shader.ID, "light_colour"), light_colour.x, light_colour.y, light_colour.z, light_colour.w);
     shaderProgram.Activate();
     glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramid_model));
     glUniform3f(glGetUniformLocation(shaderProgram.ID, "light_position"), light_position.x, light_position.y, light_position.z);
     glUniform3f(glGetUniformLocation(shaderProgram.ID, "lit_colour"), 1.0f, 0.8f, 0.6f);    // warm lit color
     glUniform3f(glGetUniformLocation(shaderProgram.ID, "shadow_colour"), 0.2f, 0.15f, 0.3f);  // cool shadow color

     glEnable(GL_DEPTH_TEST);

     Camera camera(window_width, window_height, glm::vec3(0.0f, 0.0f, 2.0f));

    //  make sure the window doesn't immediately close
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.161f, 0.118f, 0.102f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.update_matrix(45.0f, 0.1f, 100.0f);
        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camera_matrix");

        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        light_shader.Activate();
        camera.Matrix(light_shader, "camera_matrix");
        light_vao.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glDeleteProgram(shaderProgram); // line #100!


    // destroy everything when the program ends
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}