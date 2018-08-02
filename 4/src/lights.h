#ifndef LIGHTS_H_
#define LIGHTS_H_

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"
#include "camera.h"



class light {
protected:
    glm::vec3 amb;
    glm::vec3 dif;
    glm::vec3 spec;
    glm::vec3 pos;
public:
    light() : amb(0, 0, 0), dif(0, 0, 0), spec(0, 0, 0), pos(0, 0, 0) {}
    void set_shader_params(GLuint prog) {
        glUniform3fv(glGetUniformLocation(prog, "light.ambient"), 1, value_ptr(amb));
        glUniform3fv(glGetUniformLocation(prog, "light.diffuse"), 1, value_ptr(dif));
        glUniform3fv(glGetUniformLocation(prog, "light.specular"), 1, value_ptr(spec));
        glUniform3fv(glGetUniformLocation(prog, "light.position"), 1, value_ptr(pos));
    }
};


class DynLamp : protected light {
    static constexpr const char * dyn_lamp_m_path = "res/sphere2.obj";
    static constexpr const char * prog_path_F = "res/pointLightF.glsl";
    static constexpr const char * prog_path_V = "res/pointLightV.glsl";
    utils::Model m;
    glm::vec3 model_scale;

    GLuint prog;
public:
    DynLamp() : m(dyn_lamp_m_path), light() , prog(0){
        amb = glm::vec3(0.2f, 0.2f, 0.2f);
        dif = glm::vec3(0.5f, 0.5f, 0.5f);
        spec = glm::vec3(1.0f, 1.0f, 1.0f);
        model_scale = glm::vec3(0.1f);
    }
    void init() {
        prog = utils::makeProg(prog_path_V, prog_path_F);
    }
    void draw2shadow_shader(GLuint prog) {
        glUseProgram(prog);
        set_shader_params(prog);
    }

    void draw2pointlight_shader(glm::mat4 &pv) {
        glUseProgram(prog);
        glUniformMatrix4fv(glGetUniformLocation(prog, "PV"), 1, GL_FALSE, glm::value_ptr(pv));
        m.draw2(prog, pos, model_scale);
    }

    void update(float t) {
        pos = glm::vec3(sin(t), 1.0, cos(t));
    }
};


class StaticLight : protected light{
public:
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightSpaceMatrix;
    float nearPlane = 1.0f;
    float farPlane = 7.5f;
    float L = -10.0f;
    float R = 10.0f;
    float B = -10.0f;
    float T = 10.0f;
    StaticLight() : light() {
        pos =  glm::vec3(-2.0f, 4.0f, -1.0f);
//        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
        lightProjection = glm::ortho(L, R, B, T, nearPlane, farPlane);
        lightView = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0, 1, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
    }
    void set_shader_depth_params(GLuint prog) {
        glUniformMatrix4fv(glGetUniformLocation(prog, "lightM"),
                                   1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    }
    void set_shader_map_params(GLuint prog) {
        set_shader_depth_params(prog);
        glUniform3fv(glGetUniformLocation(prog, "lightPos"), 1, glm::value_ptr(pos));
    }
    float getFOVRad() {
        return atanf(T / nearPlane) * 2.0f;
    }
    void assignCamera(Camera &c,
            int width, int height) {
        pos = c.getPosition();
        lightView = c.getViewMatrix();
        lightProjection = c.getPerspective(width, height);
        lightSpaceMatrix = lightProjection * lightView;
    }
};



#endif
