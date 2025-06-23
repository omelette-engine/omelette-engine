#include "Camera.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "VBO.h"
#include "World.h"
#include "omelette_style.h"
#include "editor/Editor.h"
#include "print_helper.h"

const unsigned int window_width = 800;
const unsigned int window_height = 800;

// Editor editor;
World world;
Editor editor(world);

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
    glEnable(GL_DEPTH_TEST);

    // ========== initialisation ==========
    
    // initialise shaders
    Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
    Shader light_shader("shaders/light.vert", "shaders/light.frag");
    
    // initialise meshes
    Mesh pyramid(vertices, indices);
    Mesh point_light(lightVertices, lightIndices);

    // light properties
    vector4 light_colour = vector4(0.831f, 0.827f, 0.820f, 1.0f);
    vector3 light_position = vector3(0.5f, 0.5f, 0.5f);

    // add objects
    // world.add_object("Pyramid", &pyramid, vector3(0.0f, 0.0f, 0.0f), default_colour, default_shadow);
    world.add_object("Point Light", &point_light, light_position, light_colour, light_colour);
    log_info("Light cube position: " + std::to_string(light_position.x) + ", " + std::to_string(light_position.y) + ", " + std::to_string(light_position.z));

    // camera
    Camera camera(window_width, window_height, vector3(0.0f, 0.0f, 2.0f));
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // editor
    editor.initialise(window);

    // ========== main loop ==========
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.161f, 0.118f, 0.102f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // camera
        camera.Inputs(window);
        camera.update_matrix(45.0f, 0.1f, 100.0f);

        // rendering objects
        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camera_matrix");
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camera_position"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "light_position"), light_position.x, light_position.y, light_position.z);

        for(const auto& obj : world.get_objects()) {
            if(obj.name != "Point Light") {
                // Set all uniforms FIRST
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, get_raw_data(obj.model_matrix));
                glUniform3f(glGetUniformLocation(shaderProgram.ID, "lit_colour"), obj.lit_colour.x, obj.lit_colour.y, obj.lit_colour.z);
                glUniform3f(glGetUniformLocation(shaderProgram.ID, "shadow_colour"), obj.shadow_colour.x, obj.shadow_colour.y, obj.shadow_colour.z);
                
                // THEN render
                obj.mesh->render();
            }
        }
        
        light_shader.Activate();
        camera.Matrix(light_shader, "camera_matrix");
        glUniform3f(glGetUniformLocation(light_shader.ID, "camera_position"), camera.Position.x, camera.Position.y, camera.Position.z);
        if(!world.get_objects().empty()) {
            const auto& light_obj = world.get_objects()[0];
            glUniformMatrix4fv(glGetUniformLocation(light_shader.ID, "model"), 1, GL_FALSE, get_raw_data(light_obj.model_matrix));
            glUniform4f(glGetUniformLocation(light_shader.ID, "light_colour"), light_colour.x, light_colour.y, light_colour.z, light_colour.w);
            light_obj.mesh->render();
        }

        editor.begin_frame();
        editor.render();
        editor.end_frame();

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