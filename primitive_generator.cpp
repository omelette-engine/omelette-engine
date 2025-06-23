#include "primitive_generator.h"
#include "print_helper.h"
#include "omelette_style.h"

Mesh* generate_cube() {
    log_info("generating cube");
    dynamic_array<Vertex> cube_vertices;
    dynamic_array<GLuint> cube_indices;
    
    // We'll build each face separately so normals are correct
    GLuint index_offset = 0;
    
    // Helper lambda to add a quad face
    auto add_face = [&](vector3 v0, vector3 v1, vector3 v2, vector3 v3, vector3 normal) {
        // Add the 4 vertices for this face
        cube_vertices.push_back(Vertex{v0, normal});
        cube_vertices.push_back(Vertex{v1, normal});
        cube_vertices.push_back(Vertex{v2, normal});
        cube_vertices.push_back(Vertex{v3, normal});
        
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

Mesh* generate_primitive(const string& primitive_type) {
    if (primitive_type == "cube") {
        return generate_cube();
    }
    
    // return nullptr if primitive type not found
    return nullptr;
}