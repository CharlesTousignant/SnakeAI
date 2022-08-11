#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "OpenGlSetup.hpp"

std::string vertexShader = R"glsl(
    #version 330 core
    
    layout (location = 0) in int state;
    out vec4 color;
    void main(){
        gl_Position =  vec4( ( mod(gl_VertexID, 16.0) / 16.0 * 1.5) - 0.75,
                               ( float(floor(float(gl_VertexID / 16.0) )) / 16.0 * 1.5 ) - 0.75, 0, 1);
        if(state == 1) {
            gl_PointSize = 17;
            color = vec4(1, 0, 0, 1);
        }
        else if(state == 2){
            gl_PointSize = 17;
            color = vec4(1, 1, 0, 1);
        }
        else if(state ==3){
            gl_PointSize = 17;
            color = vec4(0, 1, 1, 1);
        }
        else gl_PointSize = 0;
    }
    )glsl";

std::string fragmentShader = R"glsl(
    #version 330 core
    
    in vec4 color;
    out vec4 fragColor;

    void main(){
        fragColor = color;
    }
    )glsl";

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment ") << "shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void SetupOpenGl() {
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);
    glEnable(GL_PROGRAM_POINT_SIZE);
}