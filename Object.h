#pragma once
#include <string>
#include "Mesh.h"
#include "AABB.h"
#include "omelette_style.h"

enum class object_type{
    MESH,
    LIGHT
};

struct Transform {
    vector3 position = vector3(0.0f);
    vector3 rotation = vector3(0.0f);  
    vector3 scale = vector3(1.0f);
};

class Object{
    public:
    std::string name;
    vector3 position;
    vector3 rotation;
    vector3 scale;
    matrix4 model_matrix;
    AABB bounding_box;
    object_type type;

    union{
        struct{
            Mesh* mesh;
            vector3 lit_colour;
            vector3 shadow_colour;
        } mesh_data;

        struct{
            Mesh* mesh;
            vector3 light_colour;
            float intensity;
        } light_data;
    };

    Object(const std::string& name, Mesh* mesh, const Transform& transform, const vector3& lit_colour, const vector3& shadow_colour, object_type obj_type = object_type::MESH) {
        mesh_data.mesh = mesh;
        mesh_data.lit_colour = lit_colour;
        mesh_data.shadow_colour = shadow_colour;

        model_matrix = matrix4(1.0f);
        model_matrix = translate_matrix(model_matrix, position);
        bounding_box = AABB::from_vertices(mesh->vertices);
    }

    Object(const std::string& name, Mesh* mesh, const Transform& transform, const vector3& position, const vector3& light_colour, float intensity) : name(name), position(position), type(object_type::LIGHT){
        light_data.mesh = mesh;
        light_data.light_colour = light_colour;
        light_data.intensity = intensity;

        model_matrix = matrix4(1.0f);
        model_matrix = translate_matrix(model_matrix, position);
        bounding_box = AABB::from_vertices(mesh->vertices);
    }

    void update_model_matrix() {
        model_matrix = matrix4(1.0f);
        model_matrix = translate_matrix(model_matrix, position);
    }

    AABB get_world_aabb() const {
        return AABB(bounding_box.min + position, bounding_box.max + position);
    }

    vector3 get_lit_colour() const {
        if (type == object_type::MESH) {
            return mesh_data.lit_colour;
        } return vector3(0.0f);
    }
    
    vector3 get_shadow_colour() const {
        if (type == object_type::MESH) {
            return mesh_data.shadow_colour;
        } return vector3(0.0f);
    }

    vector3 get_light_colour() const {
        if (type == object_type::LIGHT){
            return light_data.light_colour;
        } return vector3(0.0f);
    }

    float get_intensity() const {
        if (type == object_type::LIGHT) {
            return light_data.intensity;
        }
        return 0.0f;
    }
    
    void set_intensity(float intensity) {
        if (type == object_type::LIGHT) {
            light_data.intensity = intensity;
        }
    }

    void set_light_colour(const vector3& colour) {
        if (type == object_type::LIGHT) {
            light_data.light_colour = colour;
        }
    }
    
    void set_lit_colour(const vector3& color) {
        if (type == object_type::MESH) {
            mesh_data.lit_colour = color;
        }
    }
    
    void set_shadow_colour(const vector3& color) {
        if (type == object_type::MESH) {
            mesh_data.shadow_colour = color;
        }
    }
};