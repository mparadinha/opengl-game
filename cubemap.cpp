#include <string>
#include <vector>

#include <GL/glew.h>
#include "stb_image.h"

#include "cubemap.h"

float skybox_vertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

CubeMap::CubeMap(std::string path, std::string extension) {
    // textures
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    // load all 6 textures (1 for each side)
    std::string str[] = {"rt", "lf", "up", "dn", "bk", "ft"};
    std::vector<std::string> faces(str, str + (sizeof(str) / sizeof(std::string)));
    int w, h, colors;
    GLenum format;
    unsigned char* data;
    for(unsigned int i = 0; i < faces.size(); i++) {
        std::string file = path + "_" + faces[i] + "." + extension;
        data = stbi_load(file.c_str(), &w, &h, &colors, 0);
        if(!data) {
            std::cerr << "[CUBEMAP] Error loading texture: " << file << std::endl;
        }
        if(colors == 1) format = GL_LUMINANCE; // greyscale image
        else if(colors == 3) format = GL_RGB; // normal 3 color channels
        else if(colors == 4) format = GL_RGBA; // normal 3 + alpha

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    }

    stbi_image_free(data); // dont forget to free memory, or else we'll leak

    // options for wrapping texture
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // we'll also need a vao and vbo to draw the cubemap onto
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(float), &skybox_vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
}

void CubeMap::render() {
    glDepthMask(GL_FALSE);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
}
