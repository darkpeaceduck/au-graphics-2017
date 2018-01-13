#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "buffers.h"

#include "camera.h"
#include "light.h"

void gbuffer_t::draw(Camera& camera, int window_width, int window_height) {
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "PV"), 1, GL_FALSE, value_ptr(camera.getPV(window_width, window_height)));

    obj.draw2(shader, model_pos, model_scale);

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gbuffer_t::bind_texs(GLuint shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pos_tex_);
    glUniform1i(glGetUniformLocation(shader, "gbuffer_pos"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, norm_tex_);
    glUniform1i(glGetUniformLocation(shader, "gbuffer_norm"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, diffuse_tex_);
    glUniform1i(glGetUniformLocation(shader, "gbuffer_diffuse"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, specular_tex_);
    glUniform1i(glGetUniformLocation(shader, "gbuffer_specular"), 3);
}


void lbuffer_t::draw(Camera& camera,
        int width, int height, int gamma,
        std::vector<light_t> &lights, gbuffer_t &gbuffer, Bounds &bounds) {
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(shader);

    gbuffer.bind_texs(shader);

    glUniform2fv(glGetUniformLocation(shader, "window"), 1, value_ptr(glm::vec2(width, height)));
    glUniform3fv(glGetUniformLocation(shader, "camera"), 1, value_ptr(camera.getPosition()));
    glUniform1f(glGetUniformLocation(shader, "gamma"), gamma);

    for (light_t& light : lights) {
        light.draw(shader, camera, width, height);
        bounds.draw();
    }


    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void lbuffer_t::bind_texs(GLuint shader) {
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, light_tex_);
    glUniform1i(glGetUniformLocation(shader, "lbuffer"), 3);
}


