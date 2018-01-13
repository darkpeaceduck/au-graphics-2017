#ifndef LIGHT_H_
#define LIGHT_H_

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "util.h"
#include "camera.h"


class light_t {
    glm::vec3 position;
    GLfloat size;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::vec3 c;
    float a;
    float p;
    float h;
    float dt;
public:
    void update_position(float t);
    light_t(float t) {
        position =   utils::rndv3();
        size = 0.6f;
        ambient = glm:: vec3();
        diffuse = utils::rnd_color();
        specular = utils::rnd_color();
        c = glm::vec3(0, 0, -2);
        a = 1;
        p = M_PI *2   * 1 * utils::rnd() ;
        h = M_PI /4 * 1;
        dt = t;
    }
    void draw(GLuint shader, Camera & camera, int width, int height);
    light_t() {}
};


#endif
