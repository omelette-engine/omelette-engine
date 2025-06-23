#include"Camera.h"
#include "imgui/imgui.h"
#include <GLFW/glfw3.h>
#include "omelette_style.h"


Camera::Camera(int width, int height, vector3 position){
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

	void Camera::update_matrix(float field_of_view, float near_plane, float far_plane){
	// Initializes matrices since otherwise they will be the null matrix
	matrix4 view = matrix4(1.0f);
	matrix4 projection = matrix4(1.0f);

	// Makes camera look in the right direction from the right position
	view = look_at(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = perspective(degrees_to_radians(field_of_view), (float)width / height, near_plane, far_plane);

	// Exports the camera matrix to the Vertex Shader
	camera_matrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform){
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, get_raw_data(camera_matrix));
}


void Camera::Inputs(GLFWwindow* window){
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if(!io.WantCaptureMouse){
		// Handles key inputs
		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
			Position += speed * Orientation;
		}
		if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
			Position -= speed * Orientation;
		}

		// Handles mouse inputs
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS){
			// Hides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Prevents camera from jumping on the first click
			if (first_click){
				glfwSetCursorPos(window, width / 2.0, height / 2.0);
				first_click = false;
			}

			// Stores the coordinates of the cursor
			double mouse_x;
			double mouse_y;
			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window, &mouse_x, &mouse_y);

			// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
			// and then "transforms" them into degrees 
			float x_rotation = sensitivity * (float)(mouse_y - (height / 2.0)) / height;
			float y_rotation = sensitivity * (float)(mouse_x - (width / 2.0)) / width;

			// Calculates upcoming vertical change in the Orientation
			vector3 new_orientation = rotate_vector(Orientation, -x_rotation, normalise(cross_product(Orientation, Up)));

			// Decides whether or not the next vertical Orientation is legal or not
			if(absolute_value(angle_between_vectors(new_orientation, Up) - 90.0f) <= 85.0f){
				Orientation = new_orientation;
			}

			// Rotates the Orientation left and right
			Orientation = rotate_vector(Orientation, -y_rotation, Up);

			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			glfwSetCursorPos(window, width / 2.0, height / 2.0);
		} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE){
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
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