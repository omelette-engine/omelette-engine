#pragma once
#include <algorithm>
#include "Object.h"
#include "omelette_style.h"

class World {
public:
    dynamic_array<Object> objects;
    
    void add_object(const std::string& name, Mesh* mesh, const vector3& position, const vector3& lit_color, const vector3& shadow_colour) {
        objects.push_back(Object(name, mesh, position, lit_color, shadow_colour));
    }

    void add_light_object(const std::string& name, Mesh* mesh, const vector3& position, const vector3& light_colour, float intensity) {
        objects.push_back(Object(name, mesh, position, light_colour, intensity));
    }
    
    const dynamic_array<Object>& get_objects() const {
        return objects;
    }

    void murder_object(const std::string& name) {
        objects.erase(
            std::remove_if(objects.begin(), objects.end(),
                [&name](const Object& obj) { return obj.name == name; }),
            objects.end()
        );
    }

    Object* get_object(const std::string& name) {
        auto it = std::find_if(objects.begin(), objects.end(),
            [&name](const Object& obj) { return obj.name == name; });
        return (it != objects.end()) ? &(*it) : nullptr;
    }
};