#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
    public:
        GLuint ID;

        Shader(const GLchar* vShaderFilename, const GLchar* fShaderFilename, const GLchar* gShaderFilename = nullptr)
        {
            // 1. Retrieve the vertex/fragment/geometry source code from filePath
            std::string vertexCode;
            std::string fragmentCode;
            std::string geometryCode;
            try
            {
                // Open files
                std::ifstream vertexShaderFile(vShaderFilename);
                std::ifstream fragmentShaderFile(fShaderFilename);
                std::stringstream vShaderStream, fShaderStream;
                // Read file's buffer contents into streams
                vShaderStream << vertexShaderFile.rdbuf();
                fShaderStream << fragmentShaderFile.rdbuf();
                // close file handlers
                vertexShaderFile.close();
                fragmentShaderFile.close();
                // Convert stream into string
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
                // If geometry shader path is present, also load a geometry shader
                if (gShaderFilename != nullptr)
                {
                    std::ifstream geometryShaderFile(gShaderFilename);
                    std::stringstream gShaderStream;
                    gShaderStream << geometryShaderFile.rdbuf();
                    geometryShaderFile.close();
                    geometryCode = gShaderStream.str();
                }
            }
            catch (std::exception e)
            {
                std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
            }
            const GLchar* vShaderCode = vertexCode.c_str();
            const GLchar* fShaderCode = fragmentCode.c_str();
            const GLchar* gShaderCode = geometryCode.c_str();
            Compile(vShaderCode, fShaderCode, gShaderFilename != nullptr ? gShaderCode : nullptr);
        }

        Shader &Use()
        {
            glUseProgram(ID);
            return *this;
        }

        void Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource = nullptr)
        {
            GLuint sVertex, sFragment, gShader;
            // Vertex Shader
            sVertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(sVertex, 1, &vertexSource, NULL);
            glCompileShader(sVertex);
            checkCompileErrors(sVertex, "VERTEX");
            // Fragment Shader
            sFragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(sFragment, 1, &fragmentSource, NULL);
            glCompileShader(sFragment);
            checkCompileErrors(sFragment, "FRAGMENT");
            // If geometry shader source code is given, also compile geometry shader
            if (geometrySource != nullptr)
            {
                gShader = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(gShader, 1, &geometrySource, NULL);
                glCompileShader(gShader);
                checkCompileErrors(gShader, "GEOMETRY");
            }
            // Shader Program
            ID = glCreateProgram();
            glAttachShader(ID, sVertex);
            glAttachShader(ID, sFragment);
            if (geometrySource != nullptr)
                glAttachShader(ID, gShader);
            glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");
            // Delete the shaders as they're linked into our program now and no longer necessery
            glDeleteShader(sVertex);
            glDeleteShader(sFragment);
            if (geometrySource != nullptr)
                glDeleteShader(gShader);
        }

        void SetFloat(const std::string& name, GLfloat value, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }

        void SetInteger(const std::string& name, GLint value, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }

        void SetVector2f(const std::string& name, GLfloat x, GLfloat y, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
        }

        void SetVector2f(const std::string& name, const glm::vec2& value, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
        }

        void SetVector3f(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }

        void SetVector3f(const std::string& name, const glm::vec3& value, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
        }

        void SetVector4f(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        }

        void SetVector4f(const std::string& name, const glm::vec4& value, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
        }

        void SetMatrix4(const std::string& name, const glm::mat4& matrix, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
        }

        void SetMatrix4v(const std::string& name, const std::vector<glm::mat4>& matrices, GLboolean useShader = false)
        {
            if (useShader)
                Use();
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), (GLsizei)matrices.size(), GL_FALSE, glm::value_ptr(matrices[0]));
        }

    private:
        void checkCompileErrors(GLuint object, std::string type)
        {
            GLint success;
            GLchar infoLog[1024];
            if (type != "PROGRAM")
            {
                glGetShaderiv(object, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(object, 1024, NULL, infoLog);
                    std::cout << "| ERROR::Shader: Compile-time error: Type: " << type << "\n"
                            << infoLog << "\n -- --------------------------------------------------- -- "
                            << std::endl;
                }
            }
            else
            {
                glGetProgramiv(object, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(object, 1024, NULL, infoLog);
                    std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                            << infoLog << "\n -- --------------------------------------------------- -- "
                            << std::endl;
                }
            }
        }
};

#endif