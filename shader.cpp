#include <string>
#include <iostream>
#include <fstream>

#include "glad.h"

#include <glm/glm.hpp>

#include "shader.h"

Shader::Shader(std::string filename, std::string dir) {
    GLuint v_shader = compile(dir + "/" + filename + ".vert" , GL_VERTEX_SHADER);
    GLuint f_shader = compile(dir + "/" + filename + ".frag", GL_FRAGMENT_SHADER);
    // TODO: allow for compute shaders and other types

    // link shaders together
    program = glCreateProgram();
    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glLinkProgram(program); // TODO: check for link errors

    // dont need the shader objs in memory after the linking the program
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    use(); // select the current program

    // because the diffuse and specular textures are always (at least so far)
    // we can set these here
    // TODO: kinda hardcoded, change it
    // tell the shader program what to what texture unit each sampler2D corresponds
    set_uniform("diffuse", 0);
    set_uniform("specular", 1);
}

Shader::~Shader() {}

void Shader::use() {
    glUseProgram(program);
}

// set uniform for 4x4 matrix
void Shader::set_uniform(std::string identifier, float* matrix) {
    GLuint location = glGetUniformLocation(program, identifier.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}
// set uniform for vec3
void Shader::set_uniform(std::string identifier, glm::vec3 vector) {
    GLuint location = glGetUniformLocation(program, identifier.c_str());
    glUniform3f(location, vector.x, vector.y, vector.z);
}
// set uniform for 1 float
void Shader::set_uniform(std::string identifier, float f) {
    GLuint location = glGetUniformLocation(program, identifier.c_str());
    glUniform1f(location, f);
}
// set uniform for 1 int
void Shader::set_uniform(std::string identifier, int i) {
    GLuint location = glGetUniformLocation(program, identifier.c_str());
    glUniform1i(location, i);
}
// set uniform for 1 unsigned int
void Shader::set_uniform(std::string identifier, unsigned int i) {
    GLuint location = glGetUniformLocation(program, identifier.c_str());
    glUniform1i(location, i);
}

GLuint Shader::compile(std::string filename, GLenum type) {
    std::string shader_str = read_shader(filename);
    const char* shader_cstr = shader_str.c_str();
    const int shader_len = shader_str.length();

    GLuint shader = glCreateShader(type); // create space in GPU
    glShaderSource(shader, 1, &shader_cstr, &shader_len);
    std::cout << "compiling " << filename << std::endl;
    glCompileShader(shader);
    check_error(shader, GL_COMPILE_STATUS);

    return shader;
}

void Shader::check_error(GLuint shader, GLuint flag) {
    GLint success = 0;
    GLchar error[1024] = {};

    glGetShaderiv(shader, flag, &success);
    if(success == GL_FALSE) {
        glGetShaderInfoLog(shader, sizeof(error), NULL, error);
        std::cerr << "[SHADER ERROR] - " << error << std::endl;
    }
}

std::string Shader::read_shader(std::string path) {
    std::string directory = path.substr(0, path.find_last_of("/") + 1);

    std::ifstream file;
    file.open(path.c_str());

    std::string out, line;
    if(file.is_open()) {
        while(file.good()) {
            getline(file, line);

            // check for pre compile includes
            if(line.find("#include") != std::string::npos) {
                std::string include = line.substr(10,
                     line.find_last_of("\"") - line.find_first_of("\"") - 1);
                out.append(read_shader(directory + include) + "\n");
            }
            else {
                out.append(line + "\n");
            }
        }
    }
    else {
        std::cerr << "Unable to open file: " << path << std::endl;
    }

    return out;
}
