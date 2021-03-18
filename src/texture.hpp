#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Texture2D
{
    public:
        GLuint ID;
        GLuint Width, Height;
        GLuint InternalFormat;
        GLuint ImageFormat;
        GLuint WrapS;
        GLuint WrapT;
        GLuint FilterMin;
        GLuint FilterMax;

        Texture2D(const GLchar* textureFilename, GLboolean alpha, GLuint wrap, GLuint filterMin, GLuint filterMax) : Width(0), Height(0),
            InternalFormat(GL_RGB), ImageFormat(GL_RGB),
            WrapS(wrap), WrapT(wrap),
            FilterMin(filterMin), FilterMax(filterMax)
        {
            if (alpha)
            {
                InternalFormat = GL_RGBA;
                ImageFormat = GL_RGBA;
            }
            // Load image
            int width, height, channels;
            unsigned char* image = stbi_load(textureFilename, &width, &height, &channels, 0);
            // Now generate texture
            glGenTextures(1, &ID);
            Generate(width, height, image);
            stbi_image_free(image);
        }

        void Generate(GLuint width, GLuint height, unsigned char* data)
        {
            Width = width;
            Height = height;
            // Create Texture
            glBindTexture(GL_TEXTURE_2D, ID);
            glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, ImageFormat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            // Set Texture wrap and filter modes
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterMin);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterMax);
            // Unbind texture
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void Bind() const
        {
            glBindTexture(GL_TEXTURE_2D, ID);
        }
};

#endif