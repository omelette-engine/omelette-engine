#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>
#include "omelette_style.h"

class EBO{
    public:
        GLuint ID;
        EBO(dynamic_array<GLuint>& indices);

        void Bind();
        void Unbind();
        void Delete();
};


#endif