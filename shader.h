#ifndef SHADER_H
#define SHADER_H

#include <string>

#if __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include "glad.h"
#endif
#include <glm/glm.hpp>

class Shader {
public:
    Shader(std::string vertex_file, std::string frag_file, std::string dir="shaders");

    void use(); // select this shader's program to use in openGL

    void set_uniform(std::string name, float* matrix);
    void set_uniform(std::string name, glm::vec3 vector);
    void set_uniform(std::string name, int i);
    void set_uniform(std::string name, unsigned int i);
    void set_uniform(std::string name, float f);

    ~Shader();

    GLuint program;

private:
    void check_error(GLuint shader, GLuint flag);
    GLuint compile(std::string filename, GLenum type);
    std::string read_shader(std::string filename);

    GLuint v_shader, f_shader;
};

#endif // SHADER_H
