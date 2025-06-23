#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
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


class Camera{
    public:
        vector3 Position;
        vector3 Orientation = vector3(0.0f, 0.0f, -1.0f);
        vector3 Up = vector3(0.0f, 1.0f, 0.0f);
        matrix4 camera_matrix = matrix4(1.0f);

        bool first_click = true;

        int width;
        int height;

        float speed = 0.1f;
        float sensitivity = 2.0f;

        Camera(int width, int height, vector3 position);

        void update_matrix(float FOVdeg, float nearPlane, float farPlane);
        void Matrix(Shader& shader, const char* uniform);
        void Inputs(GLFWwindow* window);
        void update_aspect_ratio(float new_width, float new_height);
        void Zoom(double y_offset);
};


#endif