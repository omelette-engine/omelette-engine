#include "shaderClass.h"
#include "omelette_style.h"
#include "print_helper.h"

string get_file_contents(const char* filename){
    input_file_stream  in(filename, file_mode::binary);
    if(in){
        string contents;
        in.seekg(0, file_mode::end);
        contents.resize(in.tellg());
        in.seekg(0, file_mode::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    log_error("Failed to open file: ", filename);
    throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile){
    string vertexCode = get_file_contents(vertexFile);
    string fragmentCode = get_file_contents(fragmentFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate(){
    glUseProgram(ID);
}

void Shader::Delete(){
    glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char* type){
    GLint hasCompiled;
    char infoLog[1024];
    if(string(type) != "PROGRAM"){
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if(hasCompiled == GL_FALSE){
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            log_error("shader compilation error for: ", type);
            log_info(infoLog);
        }
    } else{
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if(hasCompiled == GL_FALSE){
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            log_error("shader linking error for: ", type, infoLog);
        }
    }
}