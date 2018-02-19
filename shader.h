#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(std::string filename, std::string dir="shaders");

    void use(); // select this shader's program to use in openGL

    void set_uniform(std::string name, float* matrix);
    void set_uniform(std::string name, glm::mat4 matrix);
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
};

#endif // SHADER_H
