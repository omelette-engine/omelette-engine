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
#include "primitive_generator.h"

const unsigned int window_width = 800;
const unsigned int window_height = 800;

World world;

struct WindowData {
    Camera* camera;
    Editor* editor;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if(data && data->camera){
        data->camera->update_aspect_ratio((float)width, float(height));
    }
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if(data && data->camera){
        data->camera->Zoom(y_offset);
    }
}

void mouse_callback(GLFWwindow* window, int button, int action, int modifiers){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_position, y_position;
        glfwGetCursorPos(window, &x_position, &y_position);
        WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if(data && data->editor){
            data->editor->handle_clicks(x_position, y_position);
        }
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
    

    // light properties
    // vector4 light_colour = vector4(0.831f, 0.827f, 0.820f, 1.0f);

    // add objects
    Mesh* default_light_mesh = generate_point_light();
    if(default_light_mesh) {
        world.add_light_object("Point Light", default_light_mesh, vector3(0.5f, 0.5f, 0.5f), light_colour, 1.0f);
    }

    // camera
    Camera camera(window_width, window_height, vector3(0.0f, 0.0f, 2.0f));
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    // editor

    Editor editor(world, camera);
    editor.initialise(window);

    WindowData window_data = {&camera, &editor};
    glfwSetWindowUserPointer(window, &window_data);

    // ========== main loop ==========
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.161f, 0.118f, 0.102f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // camera
        camera.Inputs(window, editor);
        camera.update_matrix(45.0f, 0.1f, 100.0f);

        // rendering objects
        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camera_matrix");
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camera_position"), camera.Position.x, camera.Position.y, camera.Position.z);

        editor.render_selected_outline(shaderProgram, camera);
        editor.render_selected_gizmos(shaderProgram, camera);
        
        vector3 actual_light_position = vector3(-1000.0f, -1000.0f, -1000.0f);
        for(const auto& obj : world.get_objects()) {
            if(obj.type == object_type::LIGHT) {
                actual_light_position = obj.position;
                break;
            }
        }
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "light_position"), actual_light_position.x, actual_light_position.y, actual_light_position.z);
        
        // render mesh objects
        for(const auto& obj : world.get_objects()) {
            if(obj.type == object_type::MESH) {
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, get_raw_data(obj.model_matrix));
                glUniform3f(glGetUniformLocation(shaderProgram.ID, "lit_colour"), obj.mesh_data.lit_colour.x, obj.mesh_data.lit_colour.y, obj.mesh_data.lit_colour.z);
                glUniform3f(glGetUniformLocation(shaderProgram.ID, "shadow_colour"), obj.mesh_data.shadow_colour.x, obj.mesh_data.shadow_colour.y, obj.mesh_data.shadow_colour.z);
                obj.mesh_data.mesh->render();
            }
        }
        
        // render light objects
        light_shader.Activate();
        camera.Matrix(light_shader, "camera_matrix");
        for(const auto& obj : world.get_objects()) {
            if(obj.type == object_type::LIGHT) {
                glUniformMatrix4fv(glGetUniformLocation(light_shader.ID, "model"), 1, GL_FALSE, get_raw_data(obj.model_matrix));
                glUniform3f(glGetUniformLocation(light_shader.ID, "light_colour"), obj.light_data.light_colour.x, obj.light_data.light_colour.y, obj.light_data.light_colour.z);
                obj.light_data.mesh->render();
            }
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