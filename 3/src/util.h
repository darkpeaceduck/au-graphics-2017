#ifndef UTIL_H
#define UTIL_H

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

    static inline float rnd() {
        return rand() * 1.0f / RAND_MAX;
    }

    static inline glm::vec3 rndv3() {
        return glm::vec3(utils::rnd(), utils::rnd(), utils::rnd());
    }

    static inline glm::vec3 rnd_color() {
        glm::vec3 color (rndv3());
        color /= color.r;
        return color;
    }
}

#endif // UTIL_H
