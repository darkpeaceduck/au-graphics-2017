#ifndef SCENE_OBJ_H_
#define SCENE_OBJ_H_

#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"

class material_obj {
protected:
    glm::vec3 amb;
    glm::vec3 dif;
    glm::vec3 spec;
    float shin;
    utils::Model m;
    glm::vec3 model_scale;
    glm::vec3 pos;
public:
    material_obj(const char * path) : amb(0, 0, 0), dif(0, 0, 0), spec(0, 0, 0), shin(0), pos(0), model_scale(0), m(path) {
    }
    void set_shader_params(GLuint prog) {
       glUniform1f(glGetUniformLocation(prog, "material.shin"), shin);
       glUniform3fv(glGetUniformLocation(prog, "material.amb"), 1, glm::value_ptr(amb));
       glUniform3fv(glGetUniformLocation(prog, "material.diff"), 1, glm::value_ptr(dif));
       glUniform3fv(glGetUniformLocation(prog, "material.spec"), 1, glm::value_ptr(spec));
    }
};


class bunny : protected material_obj {
//    static constexpr const char * bunny_m_path = "res/stanford-bunny.obj";
    static constexpr const char * bunny_m_path = "res/sponza.obj";
    float speed;
public:
    bunny() : material_obj(bunny_m_path) {
        shin = 0.2f;
        amb = glm::vec3(0.2125f, 0.1275f, 0.054f);
        dif = glm::vec3(0.714f, 0.4284f, 0.18144f);
        spec = glm::vec3(0.393548f, 0.271906f, 0.166721f);
        pos = glm::vec3(0, 0.264f, 0);
        model_scale = glm::vec3(0.0003f);
        speed = 2;
    }
    void draw(GLuint prog) {
        glUseProgram(prog);
        set_shader_params(prog);
        m.draw2(prog, pos, model_scale);
    }


    void update(float t) {
        pos = glm::vec3(cos(speed * t), pos.y, sin(speed * t));
    }
};


class scene_floor : protected material_obj {
    static constexpr const char * m_path = "res/floor.obj";
    public:
    scene_floor() : material_obj(m_path) {
        shin = 32.0f;
        amb = glm::vec3(1.0f, 0.5f, 0.31f);
        dif = glm::vec3(1.0f, 0.5f, 0.31f);
        spec = glm::vec3(0.5f, 0.5f, 0.5f);
        model_scale = glm::vec3(3.0f);
        pos = glm::vec3(0.0f, 1.5f, 0.0f);
    }
    void draw(GLuint prog) {
        set_shader_params(prog);
        m.draw2(prog, pos, model_scale);
    }
};


#endif
