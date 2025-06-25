#include"Camera.h"
#include "editor/Editor.h"
#include "imgui/imgui.h"
#include <GLFW/glfw3.h>
#include "omelette_style.h"
#include <algorithm>
#include "AABB.h"
#include "Object.h"
#include "print_helper.h"

bool f_key_was_pressed = false;


Camera::Camera(int width, int height, vector3 position){
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

	void Camera::update_matrix(float field_of_view, float near_plane, float far_plane){
	// Initializes matrices since otherwise they will be the null matrix
	matrix4 view = matrix4(1.0f);
	matrix4 projection = matrix4(1.0f);

	view = look_at(Position, Position + Orientation, Up);

	projection = perspective(degrees_to_radians(field_of_view), (float)width / height, near_plane, far_plane);

	camera_matrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform){
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, get_raw_data(camera_matrix));
}


void Camera::Inputs(GLFWwindow* window, Editor& editor){
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if(!io.WantCaptureMouse){
        // handles key inputs
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
            Position += speed * Orientation;
        }
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
            Position -= speed * Orientation;
        }
        
        bool f_key_is_pressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
        if (f_key_is_pressed && !f_key_was_pressed) {
            if (editor.has_selection()) {
                auto* selected_obj = editor.world.get_object(editor.selected_object_name);
                if (selected_obj) {
                    frame_selected_object(*selected_obj);
                }
            }
        }
        f_key_was_pressed = f_key_is_pressed;

        // orbit camera
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            double mouse_x, mouse_y;
            glfwGetCursorPos(window, &mouse_x, &mouse_y);

            if (!first_click) {
                float delta_x = sensitivity * (float)(mouse_x - last_mouse_x) / width;
                float delta_y = sensitivity * (float)(mouse_y - last_mouse_y) / height;

                orbit_yaw += delta_x;
                orbit_pitch += delta_y;
                
                orbit_pitch = std::max(-1.5f, std::min(1.5f, orbit_pitch));
                update_orbit_position();
            } else {
                orbit_distance = length(Position - pivot_point);

                vector3 to_camera = Position - pivot_point;
                orbit_yaw = atan2(to_camera.z, to_camera.x);
                orbit_pitch = asin(to_camera.y / orbit_distance);
    
                first_click = false;
            }
            
            last_mouse_x = mouse_x;
            last_mouse_y = mouse_y;
    
} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    first_click = true;
}
    }
}

void Camera::update_aspect_ratio(float new_width, float new_height){
	width = int(new_width);
	height = int(new_height);
	float aspect_ratio = new_width / new_height;
}

void Camera::Zoom(double y_offset){
	Position += (float)y_offset * speed * Orientation;
}

void Camera::frame_selected_object(const Object& obj) {
    AABB world_aabb = obj.get_world_aabb();
    pivot_point = (world_aabb.min + world_aabb.max) * 0.5f;
    
    vector3 size = world_aabb.max - world_aabb.min;
    float max_dimension = std::max({size.x, size.y, size.z});
    orbit_distance = max_dimension * 2.0f;
    
    update_orbit_position();
}


void Camera::update_orbit_position(){
	Position.x = pivot_point.x + orbit_distance * cos(orbit_pitch) * cos(orbit_yaw);
    Position.y = pivot_point.y + orbit_distance * sin(orbit_pitch);
    Position.z = pivot_point.z + orbit_distance * cos(orbit_pitch) * sin(orbit_yaw);
        
    Orientation = normalise(pivot_point - Position);
}