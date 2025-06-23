#pragma once
#include "Mesh.h"
#include "omelette_style.h"
#include <string>

class Object {
public:
    std::string name;
    Mesh* mesh;
    vector3 position;
    vector3 lit_colour;
    vector3 shadow_colour;
    matrix4 model_matrix;
    
    Object(const std::string& object_name, Mesh* object_mesh, const vector3& object_position, const vector3& lit_colour, const vector3& shadow_colour) 
    : name(object_name), mesh(object_mesh), position(object_position) {
    this->lit_colour = lit_colour;
    this->shadow_colour = shadow_colour;
    model_matrix = matrix4(1.0f);
    model_matrix = translate_matrix(model_matrix, position);
    }
    
    void update_model_matrix() {
        model_matrix = matrix4(1.0f);
        model_matrix = translate_matrix(model_matrix, position);
    }
};