#include "Camera.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "VBO.h"
#include "no_abbreviations.h"
// #include "print_helper.h"

const unsigned int window_width = 800;
const unsigned int window_height = 800;

dynamic_array<Vertex> vertices = {
    // Bottom face (y = 0)
    Vertex{vector3(-0.5f, 0.0f,  0.5f), vector3( 0.0f, -1.0f,  0.0f), vector3(0.83f, 0.70f, 0.44f)}, // 0
    Vertex{vector3(-0.5f, 0.0f, -0.5f), vector3( 0.0f, -1.0f,  0.0f), vector3(0.83f, 0.70f, 0.44f)}, // 1
    Vertex{vector3( 0.5f, 0.0f, -0.5f), vector3( 0.0f, -1.0f,  0.0f), vector3(0.83f, 0.70f, 0.44f)}, // 2
    Vertex{vector3( 0.5f, 0.0f,  0.5f), vector3( 0.0f, -1.0f,  0.0f), vector3(0.83f, 0.70f, 0.44f)}, // 3

    // Apex vertex (for all triangular faces)
    Vertex{vector3( 0.0f, 0.8f,  0.0f), vector3( 0.0f,  1.0f,  0.0f), vector3(0.92f, 0.86f, 0.76f)}, // 4

    // Left face vertices (reuse base vertices but with correct normals)
    Vertex{vector3(-0.5f, 0.0f,  0.5f), vector3(-0.707f, 0.5f,  0.5f), vector3(0.83f, 0.70f, 0.44f)}, // 5
    Vertex{vector3(-0.5f, 0.0f, -0.5f), vector3(-0.707f, 0.5f, -0.5f), vector3(0.83f, 0.70f, 0.44f)}, // 6
    Vertex{vector3( 0.0f, 0.8f,  0.0f), vector3(-0.707f, 0.707f, 0.0f), vector3(0.92f, 0.86f, 0.76f)}, // 7

    // Back face vertices
    Vertex{vector3(-0.5f, 0.0f, -0.5f), vector3( 0.0f, 0.5f, -0.707f), vector3(0.83f, 0.70f, 0.44f)}, // 8
    Vertex{vector3( 0.5f, 0.0f, -0.5f), vector3( 0.0f, 0.5f, -0.707f), vector3(0.83f, 0.70f, 0.44f)}, // 9
    Vertex{vector3( 0.0f, 0.8f,  0.0f), vector3( 0.0f, 0.707f, -0.707f), vector3(0.92f, 0.86f, 0.76f)}, // 10

    // Right face vertices  
    Vertex{vector3( 0.5f, 0.0f, -0.5f), vector3( 0.707f, 0.5f, -0.5f), vector3(0.83f, 0.70f, 0.44f)}, // 11
    Vertex{vector3( 0.5f, 0.0f,  0.5f), vector3( 0.707f, 0.5f,  0.5f), vector3(0.83f, 0.70f, 0.44f)}, // 12
    Vertex{vector3( 0.0f, 0.8f,  0.0f), vector3( 0.707f, 0.707f, 0.0f), vector3(0.92f, 0.86f, 0.76f)}, // 13

    // Front face vertices
    Vertex{vector3( 0.5f, 0.0f,  0.5f), vector3( 0.0f, 0.5f,  0.707f), vector3(0.83f, 0.70f, 0.44f)}, // 14
    Vertex{vector3(-0.5f, 0.0f,  0.5f), vector3( 0.0f, 0.5f,  0.707f), vector3(0.83f, 0.70f, 0.44f)}, // 15
    Vertex{vector3( 0.0f, 0.8f,  0.0f), vector3( 0.0f, 0.707f, 0.707f), vector3(0.92f, 0.86f, 0.76f)}  // 16
};

dynamic_array<GLuint> indices = {
    // Bottom face (two triangles)
    0, 1, 2,
    0, 2, 3,
    
    // Side faces (each is one triangle)
    5, 7, 6,   // Left face
    8, 10, 9,  // Back face  
    11, 13, 12, // Right face
    14, 16, 15  // Front face
};

dynamic_array<Vertex> lightVertices = {
	Vertex{vector3(-0.1f, -0.1f,  0.1f)},
	Vertex{vector3(-0.1f, -0.1f, -0.1f)},
	Vertex{vector3(0.1f, -0.1f, -0.1f)},
	Vertex{vector3(0.1f, -0.1f,  0.1f)},
	Vertex{vector3(-0.1f,  0.1f,  0.1f)},
	Vertex{vector3(-0.1f,  0.1f, -0.1f)},
	Vertex{vector3(0.1f,  0.1f, -0.1f)},
	Vertex{vector3(0.1f,  0.1f,  0.1f)}
};

dynamic_array<GLuint> lightIndices = {
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if(camera){
        camera -> update_aspect_ratio((float)width, float(height));
    }
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if(camera){
        camera -> Zoom(y_offset);
    }
}


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
        glfwTerminate();
        return -1;
    }

    // even though we told the computer to make a window, we still have to tell it to _show_ the window
     glfwMakeContextCurrent(window);
     gladLoadGL();
     

     glViewport(0, 0, window_width, window_height);

     Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
     Mesh pyramid(vertices, indices);

     Shader light_shader("shaders/light.vert", "shaders/light.frag");
     Mesh point_light(lightVertices, lightIndices);

     vector4 light_colour = vector4(1.0f, 1.0f, 1.0f, 1.0f);


     vector3 light_position = vector3(0.5f, 0.5f, 0.5f);
     matrix4 light_model = matrix4(1.0f);
     light_model = translate_matrix(light_model, light_position);

     vector3 pyramid_position = vector3(0.0f, 0.0f, 0.0f);
     matrix4 pyramid_model = matrix4(1.0f);
     pyramid_model = translate_matrix(pyramid_model, pyramid_position);

     light_shader.Activate();
     glUniformMatrix4fv(glGetUniformLocation(light_shader.ID, "model"), 1, GL_FALSE, get_raw_data(light_model));
     glUniform4f(glGetUniformLocation(light_shader.ID, "light_colour"), light_colour.x, light_colour.y, light_colour.z, light_colour.w);
     shaderProgram.Activate();
     glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, get_raw_data(pyramid_model));
     glUniform3f(glGetUniformLocation(shaderProgram.ID, "light_position"), light_position.x, light_position.y, light_position.z);
     glUniform3f(glGetUniformLocation(shaderProgram.ID, "lit_colour"), 1.0f, 0.8f, 0.6f);    // warm lit color
     glUniform3f(glGetUniformLocation(shaderProgram.ID, "shadow_colour"), 0.2f, 0.15f, 0.3f);  // cool shadow color

     glEnable(GL_DEPTH_TEST);

     Camera camera(window_width, window_height, vector3(0.0f, 0.0f, 2.0f));
     glfwSetWindowUserPointer(window, &camera);
     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    //  editor!
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool draw_pyramid = true;


    //  make sure the window doesn't immediately close
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.161f, 0.118f, 0.102f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // scene view
        camera.Inputs(window);
        camera.update_matrix(45.0f, 0.1f, 100.0f);
        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camera_matrix");

        if(draw_pyramid){
            pyramid.Draw(shaderProgram, camera);
        }
        point_light.Draw(light_shader, camera);



        // editor
        ImGui::Begin("my name is omelette, i'm an omelette");
        ImGui::Text("hello there omeletteer!");
        ImGui::Checkbox("wanna see a pyramid?", &draw_pyramid);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glDeleteProgram(shaderProgram); // line #100!


    // destroy everything when the program ends
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    shaderProgram.Delete();
    light_shader.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}