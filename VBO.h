#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include "no_abbreviations.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

struct Vertex {
    vector3 position;
    vector3 normal;
    vector3 colour;
};


class VBO{
    public:
        GLuint ID;
        VBO(dynamic_array<Vertex>& vertices);

        void Bind();
        void Unbind();
        void Delete();
};


#endif