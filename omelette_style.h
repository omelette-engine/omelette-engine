#pragma once
#include <vector>
#include<string>
#define GLM_ENABLE_EXPERIMENTAL
#define constant_expression constexpr
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <fstream>


using vector2 = glm::vec2;
using vector3 = glm::vec3;
using vector4 = glm::vec4;
using matrix3 = glm::mat3;
using matrix4 = glm::mat4;
using quaternion = glm::quat;

using string = std::string;
using input_file_stream = std::ifstream;
using file_mode = std::ios;

template<typename type>
using dynamic_array = std::vector<type>;


inline matrix4 look_at(const vector3& eye, const vector3& centre, const vector3& up){
    return glm::lookAt(eye, centre, up);
}

inline matrix4 perspective(float field_of_view, float aspect_ratio, float near_plane, float far_plane){
    return glm::perspective(field_of_view, aspect_ratio, near_plane, far_plane);
}

inline float degrees_to_radians(float degrees){
    return glm::radians(degrees);
}

inline float radians_to_degrees(float radians){
    return glm::degrees(radians);
}

template<typename data_type>
auto get_raw_data(const data_type& glm_object){
    return glm::value_ptr(glm_object);
}

inline matrix4 rotate_matrix(const matrix4& matrix, float angle, const vector3& axis){
    return glm::rotate(matrix, degrees_to_radians(angle), axis);
}

inline vector3 rotate_vector(const vector3& vector, float angle, const vector3& axis){
    return glm::rotate(vector, degrees_to_radians(angle), axis);
}

inline vector3 normalise(const vector3& vector){
    return glm::normalize(vector);
}

inline vector3 cross_product(const vector3& first_vector, const vector3& second_vector){
    return glm::cross(first_vector, second_vector);
}

inline float absolute_value(float value){
    return glm::abs(value);
}

inline float angle_between_vectors(const vector3& first_vector, const vector3 second_vector){
    return radians_to_degrees(glm::angle(first_vector, second_vector));
}

inline matrix4 translate_matrix(const matrix4& matrix, const vector3& translation){
    return glm::translate(matrix, translation);
}

inline float get_percentage(float percentage){
    return percentage / 100.0f;
}

const vector3 default_colour = vector3(0.286f, 0.400f, 0.412f);
const vector3 default_shadow = vector3(0.224f, 0.243f, 0.149f);
const vector3 light_colour = vector3(0.851f, 0.643f, 0.263f);


//   pretty colours :)
// #291e1a		(0.161f, 0.118f, 0.102f, 1.0f)
// #5a3428		(0.353f, 0.204f, 0.157f, 1.0f)
// #a6652b		(0.651f, 0.396f, 0.169f, 1.0f)
// #d9a443		(0.851f, 0.643f, 0.263f, 1.0f)
// #c95937		(0.788f, 0.349f, 0.216f, 1.0f)
// #893f43		(0.537f, 0.247f, 0.263f, 1.0f)
// #393e26		(0.224f, 0.243f, 0.149f, 1.0f)
// #496669		(0.286f, 0.400f, 0.412f, 1.0f)
// #7687ab		(0.463f, 0.529f, 0.671f, 1.0f)
// #d4d3d1		(0.831f, 0.827f, 0.820f, 1.0f)
// #748b2c		(0.455f, 0.545f, 0.173f, 1.0f)
// #31542f		(0.192f, 0.329f, 0.184f, 1.0f) 