//
// Created by matf-rg on 4.12.20..
//

#ifndef PROJECT_BASE_MODEL_H
#define PROJECT_BASE_MODEL_H
#include <stb_image.h>
#include <vector>
#include <string>
#include <learnopengl/shader.h>
#include <rg/mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <rg/Error.h>
class Model {
public:
    std::vector<Mesh> meshes;
    std::vector<Texture> loaded_textures;

    std::string directory;
    Model(std::string path) {

        loadModel(path);
    }

    void Draw(Shader& shader) {
        for (Mesh& mesh : meshes) {
            mesh.Draw(shader);
        }
    }

private:
    void loadModel(std::string path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
        aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            ASSERT(false, "Failed to load a model!");
            return;
        }
        this->directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Vertex vertex;
            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;
            
            if (mesh->HasNormals()) {
                vertex.Normal.x = mesh->mNormals[i].x;
                vertex.Normal.y = mesh->mNormals[i].y;
                vertex.Normal.z = mesh->mNormals[i].z;
            }

            if (mesh->mTextureCoords[0]) {
                vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;

                vertex.Tangent.x = mesh->mTangents[i].x;
                vertex.Tangent.y = mesh->mTangents[i].y;
                vertex.Tangent.z = mesh->mTangents[i].z;
                
                vertex.Bitangent.x = mesh->mBitangents[i].x;
                vertex.Bitangent.y = mesh->mBitangents[i].y;
                vertex.Bitangent.z = mesh->mBitangents[i].z;

            } else {
                vertex.TexCoords = glm::vec2(0.0);
            }
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mFaces; ++i) {
            aiFace face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }


        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        loadTextureMaterial(material, aiTextureType_DIFFUSE, "texture_diffuse", textures);

        loadTextureMaterial(material, aiTextureType_SPECULAR, "texture_specular",
                                                                textures);

        loadTextureMaterial(material, aiTextureType_NORMALS, "texture_normal", textures);

        loadTextureMaterial(material, aiTextureType_HEIGHT, "texture_height",
                                                              textures);


        return Mesh(vertices, indices, textures);
    }

    void loadTextureMaterial(aiMaterial* mat, aiTextureType type, std::string typeName,
                                             std::vector<Texture> & textures) {

        for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;

            for (unsigned int j = 0; j < loaded_textures.size(); ++j) {
                if (std::strcmp(str.C_Str(), loaded_textures[j].path.c_str()) == 0) {
                    textures.push_back(loaded_textures[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                loaded_textures.push_back(texture);
            }
        }

    }
};

unsigned int TextureFromFile(const char* filename, std::string directory) {
    std::string fullPath(directory + "/" + filename);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_FILTER, GL_LINEAR);

    } else {
        ASSERT(false, "Failed to load texture image");
    }
    stbi_image_free(data);
    return textureID;
}

#endif //PROJECT_BASE_MODEL_H
