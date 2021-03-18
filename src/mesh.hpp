#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/matrix4x4.h>

#include "shader.hpp"

const unsigned int NUM_BONES_PER_VERTEX = 4;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::ivec4 BoneIDs;
    glm::vec4 BoneWeights;
};

struct Texture
{
    unsigned int ID;
    std::string Type;
    std::string Path;
};

class Mesh
{
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
            vertices(vertices),
            indices(indices),
            textures(textures)
        {
            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh();
        }
        ~Mesh() {}

        void Draw(Shader shader)
        {
            // bind appropriate textures
            unsigned int diffuseNr  = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr   = 1;
            unsigned int emissionNr = 1;
            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                // retrieve texture number (the N in diffuse_textureN)
                std::string number;
                std::string name = textures[i].Type;
                if(name == "texture_diffuse")
                    number = std::to_string(diffuseNr++); // transfer unsigned int to stream
                else if(name == "texture_specular")
                    number = std::to_string(specularNr++);
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++);
                else if (name == "texture_emission")
                    number = std::to_string(emissionNr++);

                // now set the sampler to the correct texture unit
                glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, textures[i].ID);
            }

            // draw mesh
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // always good practice to set everything back to defaults once configured.
            glActiveTexture(GL_TEXTURE0);
        }

    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        unsigned int VAO, VBO, EBO;

        // initializes all the buffer objects/arrays
        void setupMesh()
        {
            // create buffers/arrays
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            // load data into vertex buffers
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // A great thing about structs is that their memory layout is sequential for all its items.
            // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
            // again translates to 3/2 floats which translates to a byte array.
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // set the vertex attribute pointers
            // vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            // vertex bone ids
            glEnableVertexAttribArray(3);
            glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));
            // vertex bone weights
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneWeights));

            glBindVertexArray(0);
        }
};
#endif