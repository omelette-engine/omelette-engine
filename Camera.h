#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#include "editor/Editor.h"
#include "omelette_style.h"
#include <glm/ext/vector_float3.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shaderClass.h"
#include "Object.h"


class Camera{
    public:
        vector3 Position;
        vector3 Orientation = vector3(0.0f, 0.0f, -1.0f);
        vector3 Up = vector3(0.0f, 1.0f, 0.0f);
        matrix4 camera_matrix = matrix4(1.0f);
        vector3 pivot_point = vector3(0.0f, 0.0f, 0.0f);
        float orbit_distance = 5.0f;
        float orbit_yaw = 0.0f;
        float orbit_pitch = 0.0f;
        double last_mouse_x, last_mouse_y;
        double virtual_mouse_x = 0.0, virtual_mouse_y = 0.0;

        bool first_click = true;
        bool just_warped = false;

        int width;
        int height;

        float speed = 0.1f;
        float sensitivity = 2.0f;

        Camera(int width, int height, vector3 position);

        void update_matrix(float FOVdeg, float nearPlane, float farPlane);
        void Matrix(Shader& shader, const char* uniform);
        void Inputs(GLFWwindow* window, Editor& editor);
        void update_aspect_ratio(float new_width, float new_height);
        void Zoom(double y_offset);
        void frame_selected_object(const Object& obj);
        void update_orbit_position();
};


#endif