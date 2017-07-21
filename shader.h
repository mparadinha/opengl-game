#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(std::string vertex_file, std::string frag_file, std::string dir="shaders");

    void use(); // select this shader's program to use in openGL
    GLuint get_program() { return program; };

    void set_uniform(std::string name, float* matrix);
    void set_uniform(std::string name, glm::vec3 vector);
    void set_uniform(std::string name, int i);
    void set_uniform(std::string name, unsigned int i);
    void set_uniform(std::string name, float f);

    ~Shader() {};

    GLuint program;

private:
    void check_error(GLuint shader, GLuint flag);
    GLuint compile(std::string filename, GLenum type);

    GLuint v_shader, f_shader;
};

std::string read_file(std::string filename);

#endif // SHADER_H
