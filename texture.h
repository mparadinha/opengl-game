#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <string>

#include "glad.h"

#include "stb_image.h"

typedef struct Texture {
    int w, h, colors;
    GLenum format;
    unsigned char* data;

    Texture(std::string path) {
        data = stbi_load(path.c_str(), &w, &h, &colors, 0);
        if(!data) {
            std::cerr << "[TEXTURE] Error loading texture: " << path << std::endl;
        }
        if(colors == 1) format = GL_LUMINANCE; // greyscale image
        else if(colors == 3) format = GL_RGB; // normal 3 color channels
        else if(colors == 4) format = GL_RGBA; // normal 3 + alpha
    }

    // load into the GPU
    unsigned int load() {
        unsigned int id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        return id;
    }

    ~Texture() {
        stbi_image_free(data);
    }

} Texture;

#endif // include guard
