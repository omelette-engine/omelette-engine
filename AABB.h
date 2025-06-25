#include <algorithm>
#include "omelette_style.h"
#include "Mesh.h"
#pragma once

struct AABB {
    vector3 min;
    vector3 max;
    
    AABB() : min(0.0f), max(0.0f) {}
    AABB(vector3 min_point, vector3 max_point) : min(min_point), max(max_point) {}
    
    // Ray-AABB intersection using the slab method
    bool intersects_ray(vector3 ray_origin, vector3 ray_direction, float& hit_distance) const {
        // Calculate intersection distances for each axis
        vector3 inv_dir = 1.0f / ray_direction;
        
        vector3 t_min = (min - ray_origin) * inv_dir;
        vector3 t_max = (max - ray_origin) * inv_dir;
        
        // handle negative direction components
        if (inv_dir.x < 0.0f) std::swap(t_min.x, t_max.x);
        if (inv_dir.y < 0.0f) std::swap(t_min.y, t_max.y);
        if (inv_dir.z < 0.0f) std::swap(t_min.z, t_max.z);
        
        // find the largest t_min and smallest t_max
        float t_near = std::max({t_min.x, t_min.y, t_min.z});
        float t_far = std::min({t_max.x, t_max.y, t_max.z});
        
        // ray misses if t_near > t_far or if intersection is behind ray origin
        if (t_near > t_far || t_far < 0.0f) {
            return false;
        }
        
        // use t_near if it's positive, otherwise use t_far
        hit_distance = (t_near >= 0.0f) ? t_near : t_far;
        return true;
    }
    
    // helper to create AABB from mesh vertices
    static AABB from_vertices(const dynamic_array<Vertex>& vertices) {
        if (vertices.empty()) return AABB();
        
        vector3 min_point = vertices[0].position;
        vector3 max_point = vertices[0].position;
        
        for (const auto& vertex : vertices) {
            min_point = glm::min(min_point, vertex.position);
            max_point = glm::max(max_point, vertex.position);
        }
        
        return AABB(min_point, max_point);
    }
};