#pragma once
#include "Object.h"
#include "omelette_style.h"

class World {
public:
    dynamic_array<Object> objects;
    
    void add_object(const std::string& name, Mesh* mesh, const vector3& position, const vector3& lit_color, const vector3& shadow_colour) {
        objects.push_back(Object(name, mesh, position, lit_color, shadow_colour));
    }
    
    const dynamic_array<Object>& get_objects() const {
        return objects;
    }
};