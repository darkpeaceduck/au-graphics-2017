#ifndef UTIL_H
#define UTIL_H
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace utils {
    GLuint makeProg(const char* vertexFilePath, const char* fragmentFilePath);

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
    };

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        void draw();
    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
    };

    class Model {
    public:
        explicit Model(const char *path);
        void draw();
        void draw2(GLuint prog, glm::vec3 pos, glm::vec3 scale);

    private:
        std::vector<Mesh> meshes;
        void loadModel(const std::string& path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh);
    };

}

#endif // UTIL_H
