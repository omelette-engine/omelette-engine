#include "primitive_generator.h"
#include "Mesh.h"
#include "VBO.h"
#include "print_helper.h"
#include "omelette_style.h"
#include <cmath>
#include <map>
#include <utility>

Mesh* generate_cube() {
    log_info("generating cube");
    dynamic_array<Vertex> cube_vertices;
    dynamic_array<GLuint> cube_indices;
    
    // We'll build each face separately so normals are correct
    GLuint index_offset = 0;
    
    // Helper lambda to add a quad face
    auto add_face = [&](vector3 v0, vector3 v1, vector3 v2, vector3 v3, vector3 normal) {
        // Add the 4 vertices for this face
        vector3 default_colour = vector3(0.788f, 0.349f, 0.216f);
        cube_vertices.push_back(Vertex{v0, normal, default_colour});
        cube_vertices.push_back(Vertex{v1, normal, default_colour});
        cube_vertices.push_back(Vertex{v2, normal, default_colour});
        cube_vertices.push_back(Vertex{v3, normal, default_colour});
        
        // Add indices for 2 triangles (quad = 2 triangles)
        cube_indices.push_back(index_offset + 0); cube_indices.push_back(index_offset + 1); cube_indices.push_back(index_offset + 2);
        cube_indices.push_back(index_offset + 0); cube_indices.push_back(index_offset + 2); cube_indices.push_back(index_offset + 3);
        
        index_offset += 4;
    };
    
    // Front face (z = 0.5)
    add_face(vector3(-0.5f, -0.5f,  0.5f), vector3( 0.5f, -0.5f,  0.5f), 
             vector3( 0.5f,  0.5f,  0.5f), vector3(-0.5f,  0.5f,  0.5f), 
             vector3(0.0f, 0.0f, 1.0f));
    
    // Back face (z = -0.5)
    add_face(vector3( 0.5f, -0.5f, -0.5f), vector3(-0.5f, -0.5f, -0.5f), 
             vector3(-0.5f,  0.5f, -0.5f), vector3( 0.5f,  0.5f, -0.5f), 
             vector3(0.0f, 0.0f, -1.0f));
    
    // Left face (x = -0.5)
    add_face(vector3(-0.5f, -0.5f, -0.5f), vector3(-0.5f, -0.5f,  0.5f), 
             vector3(-0.5f,  0.5f,  0.5f), vector3(-0.5f,  0.5f, -0.5f), 
             vector3(-1.0f, 0.0f, 0.0f));
    
    // Right face (x = 0.5)
    add_face(vector3( 0.5f, -0.5f,  0.5f), vector3( 0.5f, -0.5f, -0.5f), 
             vector3( 0.5f,  0.5f, -0.5f), vector3( 0.5f,  0.5f,  0.5f), 
             vector3(1.0f, 0.0f, 0.0f));
    
    // Bottom face (y = -0.5)
    add_face(vector3(-0.5f, -0.5f, -0.5f), vector3( 0.5f, -0.5f, -0.5f), 
             vector3( 0.5f, -0.5f,  0.5f), vector3(-0.5f, -0.5f,  0.5f), 
             vector3(0.0f, -1.0f, 0.0f));
    
    // Top face (y = 0.5)
    add_face(vector3(-0.5f,  0.5f,  0.5f), vector3( 0.5f,  0.5f,  0.5f), 
             vector3( 0.5f,  0.5f, -0.5f), vector3(-0.5f,  0.5f, -0.5f), 
             vector3(0.0f, 1.0f, 0.0f));
    
    return new Mesh(cube_vertices, cube_indices);
}

Mesh* generate_cone() {
    log_info("generating cone");
    dynamic_array<Vertex> cone_vertices;
    dynamic_array<GLuint> cone_indices;
    
    const float radius = 0.5f;
    const float height = 1.0f;
    const float half_height = height * 0.5f;
    int segments = 32;
    
    GLuint index_offset = 0;
    
    // Apex of the cone (top center)
    vector3 apex(0.0f, half_height, 0.0f);
    
    // Center of the base (bottom center)
    vector3 base_center(0.0f, -half_height, 0.0f);
    
    // Generate base circle vertices
    dynamic_array<vector3> base_positions;
    for (int i = 0; i < segments; i++) {
        float angle = (float)i / (float)segments * 2.0f * M_PI;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        base_positions.push_back(vector3(x, -half_height, z));
    }
    
    // Generate side faces (triangles from apex to base edge)
    for (int i = 0; i < segments; i++) {
        int next_i = (i + 1) % segments;
        
        vector3 v0 = apex;
        vector3 v1 = base_positions[i];
        vector3 v2 = base_positions[next_i];
        
        // Calculate normal for this triangle face
        vector3 edge1 = v1 - v0;
        vector3 edge2 = v2 - v0;
        vector3 normal = normalize(cross(edge1, edge2));
        
        // Add vertices for this triangle
        cone_vertices.push_back(Vertex{v0, normal, default_colour});
        cone_vertices.push_back(Vertex{v1, normal, default_colour});
        cone_vertices.push_back(Vertex{v2, normal, default_colour});
        
        // Add indices for the triangle
        cone_indices.push_back(index_offset + 0);
        cone_indices.push_back(index_offset + 1);
        cone_indices.push_back(index_offset + 2);
        
        index_offset += 3;
    }
    
    // Generate base face (circle made of triangles)
    vector3 base_normal(0.0f, -1.0f, 0.0f); // Points downward
    
    for (int i = 0; i < segments; i++) {
        int next_i = (i + 1) % segments;
        
        vector3 v0 = base_center;
        vector3 v1 = base_positions[next_i]; // Note: reversed order for correct winding
        vector3 v2 = base_positions[i];
        
        // Add vertices for this triangle
        cone_vertices.push_back(Vertex{v0, base_normal, default_colour});
        cone_vertices.push_back(Vertex{v1, base_normal, default_colour});
        cone_vertices.push_back(Vertex{v2, base_normal, default_colour});
        
        // Add indices for the triangle
        cone_indices.push_back(index_offset + 0);
        cone_indices.push_back(index_offset + 1);
        cone_indices.push_back(index_offset + 2);
        
        index_offset += 3;
    }
    
    return new Mesh(cone_vertices, cone_indices);
}

Mesh* generate_cylinder() {
    log_info("generating cylinder");
    dynamic_array<Vertex> cylinder_vertices;
    dynamic_array<GLuint> cylinder_indices;
    
    const float radius = 0.5f;
    const float height = 1.0f;
    const float half_height = height * 0.5f;
    int segments = 32;
    
    GLuint index_offset = 0;
    
    // Centers of the top and bottom faces
    vector3 top_center(0.0f, half_height, 0.0f);
    vector3 bottom_center(0.0f, -half_height, 0.0f);
    
    // Generate circle vertices for top and bottom
    dynamic_array<vector3> top_positions;
    dynamic_array<vector3> bottom_positions;
    
    for (int i = 0; i < segments; i++) {
        float angle = (float)i / (float)segments * 2.0f * M_PI;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        
        top_positions.push_back(vector3(x, half_height, z));
        bottom_positions.push_back(vector3(x, -half_height, z));
    }
    
    // Generate side faces (quads connecting top and bottom circles)
    for (int i = 0; i < segments; i++) {
        int next_i = (i + 1) % segments;
        
        vector3 top_current = top_positions[i];
        vector3 top_next = top_positions[next_i];
        vector3 bottom_current = bottom_positions[i];
        vector3 bottom_next = bottom_positions[next_i];
        
        // Calculate outward normal for this side face
        vector3 radial_dir = normalize(vector3(top_current.x, 0.0f, top_current.z));
        
        // Add quad as two triangles
        // Triangle 1: bottom_current, top_current, top_next
        cylinder_vertices.push_back(Vertex{bottom_current, radial_dir, default_colour});
        cylinder_vertices.push_back(Vertex{top_current, radial_dir, default_colour});
        cylinder_vertices.push_back(Vertex{top_next, radial_dir, default_colour});
        
        cylinder_indices.push_back(index_offset + 0);
        cylinder_indices.push_back(index_offset + 1);
        cylinder_indices.push_back(index_offset + 2);
        index_offset += 3;
        
        // Triangle 2: bottom_current, top_next, bottom_next
        cylinder_vertices.push_back(Vertex{bottom_current, radial_dir, default_colour});
        cylinder_vertices.push_back(Vertex{top_next, radial_dir, default_colour});
        cylinder_vertices.push_back(Vertex{bottom_next, radial_dir, default_colour});
        
        cylinder_indices.push_back(index_offset + 0);
        cylinder_indices.push_back(index_offset + 1);
        cylinder_indices.push_back(index_offset + 2);
        index_offset += 3;
    }
    
    // Generate top face (circle made of triangles)
    vector3 top_normal(0.0f, 1.0f, 0.0f); // Points upward
    
    for (int i = 0; i < segments; i++) {
        int next_i = (i + 1) % segments;
        
        vector3 v0 = top_center;
        vector3 v1 = top_positions[i];
        vector3 v2 = top_positions[next_i];
        
        // Add vertices for this triangle
        cylinder_vertices.push_back(Vertex{v0, top_normal, default_colour});
        cylinder_vertices.push_back(Vertex{v1, top_normal, default_colour});
        cylinder_vertices.push_back(Vertex{v2, top_normal, default_colour});
        
        // Add indices for the triangle
        cylinder_indices.push_back(index_offset + 0);
        cylinder_indices.push_back(index_offset + 1);
        cylinder_indices.push_back(index_offset + 2);
        
        index_offset += 3;
    }
    
    // Generate bottom face (circle made of triangles)
    vector3 bottom_normal(0.0f, -1.0f, 0.0f); // Points downward
    
    for (int i = 0; i < segments; i++) {
        int next_i = (i + 1) % segments;
        
        vector3 v0 = bottom_center;
        vector3 v1 = bottom_positions[next_i]; // Note: reversed order for correct winding
        vector3 v2 = bottom_positions[i];
        
        // Add vertices for this triangle
        cylinder_vertices.push_back(Vertex{v0, bottom_normal, default_colour});
        cylinder_vertices.push_back(Vertex{v1, bottom_normal, default_colour});
        cylinder_vertices.push_back(Vertex{v2, bottom_normal, default_colour});
        
        // Add indices for the triangle
        cylinder_indices.push_back(index_offset + 0);
        cylinder_indices.push_back(index_offset + 1);
        cylinder_indices.push_back(index_offset + 2);
        
        index_offset += 3;
    }
    
    return new Mesh(cylinder_vertices, cylinder_indices);
}

Mesh* generate_icosphere(int subdivisions = 2) {
    log_info("generating icosphere");
    dynamic_array<Vertex> icosphere_vertices;
    dynamic_array<GLuint> icosphere_indices;
    
    const float radius = 0.5f;
    
    // Golden ratio
    const float phi = (1.0f + sqrt(5.0f)) / 2.0f;
    const float inv_norm = 1.0f / sqrt(phi * phi + 1.0f);
    
    // Create the 12 vertices of the icosahedron
    dynamic_array<vector3> vertices = {
        // Top cap
        vector3(0, phi, 1) * inv_norm,
        vector3(0, phi, -1) * inv_norm,
        vector3(0, -phi, 1) * inv_norm,
        vector3(0, -phi, -1) * inv_norm,
        
        // Middle ring 1
        vector3(1, 0, phi) * inv_norm,
        vector3(-1, 0, phi) * inv_norm,
        vector3(1, 0, -phi) * inv_norm,
        vector3(-1, 0, -phi) * inv_norm,
        
        // Middle ring 2
        vector3(phi, 1, 0) * inv_norm,
        vector3(phi, -1, 0) * inv_norm,
        vector3(-phi, 1, 0) * inv_norm,
        vector3(-phi, -1, 0) * inv_norm
    };
    
    // Create the 20 triangular faces of the icosahedron
    dynamic_array<vector3> faces = {
        // Top cap triangles
        vector3(0, 8, 4), vector3(0, 4, 5), vector3(0, 5, 10), vector3(0, 10, 1), vector3(0, 1, 8),
        
        // Middle band triangles
        vector3(4, 8, 9), vector3(5, 4, 2), vector3(10, 5, 11), vector3(1, 10, 7), vector3(8, 1, 6),
        vector3(9, 8, 6), vector3(2, 4, 9), vector3(11, 5, 2), vector3(7, 10, 11), vector3(6, 1, 7),
        
        // Bottom cap triangles
        vector3(3, 9, 6), vector3(3, 2, 9), vector3(3, 11, 2), vector3(3, 7, 11), vector3(3, 6, 7)
    };
    
    // Function to get midpoint of two vertices on unit sphere
    auto get_midpoint = [&](int i1, int i2, std::map<std::pair<int, int>, int>& midpoint_cache) -> int {
        std::pair<int, int> key = std::make_pair(std::min(i1, i2), std::max(i1, i2));
        
        auto it = midpoint_cache.find(key);
        if (it != midpoint_cache.end()) {
            return it->second;
        }
        
        vector3 midpoint = normalize((vertices[i1] + vertices[i2]) * 0.5f);
        vertices.push_back(midpoint);
        int index = vertices.size() - 1;
        midpoint_cache[key] = index;
        return index;
    };
    
    // Subdivide triangles
    for (int sub = 0; sub < subdivisions; sub++) {
        dynamic_array<vector3> new_faces;
        std::map<std::pair<int, int>, int> midpoint_cache;
        
        for (const auto& face : faces) {
            int v1 = (int)face.x;
            int v2 = (int)face.y;
            int v3 = (int)face.z;
            
            // Get midpoints
            int a = get_midpoint(v1, v2, midpoint_cache);
            int b = get_midpoint(v2, v3, midpoint_cache);
            int c = get_midpoint(v3, v1, midpoint_cache);
            
            // Create 4 new triangles
            new_faces.push_back(vector3(v1, a, c));
            new_faces.push_back(vector3(v2, b, a));
            new_faces.push_back(vector3(v3, c, b));
            new_faces.push_back(vector3(a, b, c));
        }
        
        faces = new_faces;
    }
    
    // Scale vertices to desired radius and create mesh
    for (const auto& vertex : vertices) {
        vector3 scaled_vertex = vertex * radius;
        vector3 normal = normalize(vertex); // For a sphere, normal = normalized position
        icosphere_vertices.push_back(Vertex{scaled_vertex, normal, default_colour});
    }
    
    // Create indices from faces
    for (const auto& face : faces) {
        icosphere_indices.push_back((GLuint)face.x);
        icosphere_indices.push_back((GLuint)face.y);
        icosphere_indices.push_back((GLuint)face.z);
    }
    
    return new Mesh(icosphere_vertices, icosphere_indices);
}

Mesh* generate_torus(int major_segments = 32, int minor_segments = 16) {
    log_info("generating torus");
    dynamic_array<Vertex> torus_vertices;
    dynamic_array<GLuint> torus_indices;
    
    const float major_radius = 0.3f;  // Distance from center to tube center
    const float minor_radius = 0.2f;  // Tube thickness radius
    
    GLuint index_offset = 0;
    
    // Generate vertices
    for (int i = 0; i < major_segments; i++) {
        float theta = (float)i / (float)major_segments * 2.0f * M_PI; // Major circle angle
        
        for (int j = 0; j < minor_segments; j++) {
            float phi = (float)j / (float)minor_segments * 2.0f * M_PI; // Minor circle angle
            
            // Calculate position
            float cos_theta = cos(theta);
            float sin_theta = sin(theta);
            float cos_phi = cos(phi);
            float sin_phi = sin(phi);
            
            // Torus parametric equations
            float x = (major_radius + minor_radius * cos_phi) * cos_theta;
            float y = minor_radius * sin_phi;
            float z = (major_radius + minor_radius * cos_phi) * sin_theta;
            
            vector3 position(x, y, z);
            
            // Calculate normal
            // Normal points from the tube center outward
            vector3 tube_center(major_radius * cos_theta, 0.0f, major_radius * sin_theta);
            vector3 normal = normalize(position - tube_center);
            
            torus_vertices.push_back(Vertex{position, normal, default_colour});
        }
    }
    
    // Generate indices (quads made of two triangles each)
    for (int i = 0; i < major_segments; i++) {
        int next_i = (i + 1) % major_segments;
        
        for (int j = 0; j < minor_segments; j++) {
            int next_j = (j + 1) % minor_segments;
            
            // Calculate vertex indices for this quad
            GLuint v0 = i * minor_segments + j;           // current major, current minor
            GLuint v1 = next_i * minor_segments + j;      // next major, current minor
            GLuint v2 = next_i * minor_segments + next_j; // next major, next minor
            GLuint v3 = i * minor_segments + next_j;      // current major, next minor
            
            // First triangle (v0, v1, v2)
            torus_indices.push_back(v0);
            torus_indices.push_back(v1);
            torus_indices.push_back(v2);
            
            // Second triangle (v0, v2, v3)
            torus_indices.push_back(v0);
            torus_indices.push_back(v2);
            torus_indices.push_back(v3);
        }
    }
    
    return new Mesh(torus_vertices, torus_indices);
}

Mesh* generate_point_light() {
    log_info("generating light cube");
    dynamic_array<Vertex> light_vertices;
    dynamic_array<GLuint> light_indices;
    
    // build each face separately so normals are correct
    GLuint index_offset = 0;
    
    // helper lambda to add a quad face
    auto add_face = [&](vector3 v0, vector3 v1, vector3 v2, vector3 v3, vector3 normal) {
        // add the 4 vertices for this face
        vector3 light_colour = vector3(0.851f, 0.643f, 0.263f); // #d9a443
        light_vertices.push_back(Vertex{v0, normal, light_colour});
        light_vertices.push_back(Vertex{v1, normal, light_colour});
        light_vertices.push_back(Vertex{v2, normal, light_colour});
        light_vertices.push_back(Vertex{v3, normal, light_colour});
        
        // add indices for 2 triangles (quad = 2 triangles)
        light_indices.push_back(index_offset + 0); light_indices.push_back(index_offset + 1); light_indices.push_back(index_offset + 2);
        light_indices.push_back(index_offset + 0); light_indices.push_back(index_offset + 2); light_indices.push_back(index_offset + 3);
        
        index_offset += 4;
    };
    
    // Front face (z = 0.1)
    add_face(vector3(-0.1f, -0.1f,  0.1f), vector3( 0.1f, -0.1f,  0.1f), 
             vector3( 0.1f,  0.1f,  0.1f), vector3(-0.1f,  0.1f,  0.1f), 
             vector3(0.0f, 0.0f, 1.0f));
    
    // Back face (z = -0.1)
    add_face(vector3( 0.1f, -0.1f, -0.1f), vector3(-0.1f, -0.1f, -0.1f), 
             vector3(-0.1f,  0.1f, -0.1f), vector3( 0.1f,  0.1f, -0.1f), 
             vector3(0.0f, 0.0f, -1.0f));
    
    // Left face (x = -0.1)
    add_face(vector3(-0.1f, -0.1f, -0.1f), vector3(-0.1f, -0.1f,  0.1f), 
             vector3(-0.1f,  0.1f,  0.1f), vector3(-0.1f,  0.1f, -0.1f), 
             vector3(-1.0f, 0.0f, 0.0f));
    
    // Right face (x = 0.1)
    add_face(vector3( 0.1f, -0.1f,  0.1f), vector3( 0.1f, -0.1f, -0.1f), 
             vector3( 0.1f,  0.1f, -0.1f), vector3( 0.1f,  0.1f,  0.1f), 
             vector3(1.0f, 0.0f, 0.0f));
    
    // Bottom face (y = -0.1)
    add_face(vector3(-0.1f, -0.1f, -0.1f), vector3( 0.1f, -0.1f, -0.1f), 
             vector3( 0.1f, -0.1f,  0.1f), vector3(-0.1f, -0.1f,  0.1f), 
             vector3(0.0f, -1.0f, 0.0f));
    
    // Top face (y = 0.1)
    add_face(vector3(-0.1f,  0.1f,  0.1f), vector3( 0.1f,  0.1f,  0.1f), 
             vector3( 0.1f,  0.1f, -0.1f), vector3(-0.1f,  0.1f, -0.1f), 
             vector3(0.0f, 1.0f, 0.0f));
    
    return new Mesh(light_vertices, light_indices);
}

Mesh* generate_primitive(const string& primitive_type) {
    if (primitive_type == "cube") {
        return generate_cube();
    } else if (primitive_type == "cone") {
        return generate_cone();
    } else if(primitive_type == "cylinder"){
        return generate_cylinder();
    } else if (primitive_type == "sphere"){
        return generate_icosphere();
    } else if(primitive_type == "torus"){
        return generate_torus();
    } else{
        log_error("that shape doesn't exist!");
    }
    
    // return nullptr if primitive type not found
    return nullptr;
}