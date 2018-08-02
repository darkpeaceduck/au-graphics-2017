#ifndef BUFFERS_H_
#define BUFFERS_H_

#include "util.h"
#include "light.h"
#include <vector>
#include "bounds.h"
#include "camera.h"

class ibuffer {
    int width, height;
#define ATTACHMENTS_MAX 10
    GLenum attachments[ATTACHMENTS_MAX];
    GLuint attachments_tex[ATTACHMENTS_MAX];
#undef ATTACHMENTS_MAX
    GLsizei attachments_sz = 0;
public:
    GLuint depth_buf_;
      GLuint fb_;
    ibuffer(int width, int height) : width(width), height(height) {
       glGenFramebuffers(1, &fb_);
       glGenRenderbuffers(1, &depth_buf_);
       glBindFramebuffer(GL_FRAMEBUFFER, fb_);

       glBindRenderbuffer(GL_RENDERBUFFER, depth_buf_);
       glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
       glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf_);
    }
    void fin_init() {
       glDrawBuffers(attachments_sz, attachments);
       glBindRenderbuffer(GL_RENDERBUFFER, 0);
       glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    GLuint attach_new_texture(GLint internalFormat, GLenum format, GLenum type, GLenum attach) {
        GLuint tex_id;
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture(GL_FRAMEBUFFER, attach, tex_id, 0);

        attachments[attachments_sz] = (attach);
        attachments_tex[attachments_sz] = (tex_id);
        attachments_sz++;
        return tex_id;
    }
    ~ibuffer() {
        for (GLsizei i = 0; i < attachments_sz; i++)
            glDeleteTextures(1, &(attachments_tex[i]));
        glDeleteRenderbuffers(1, &depth_buf_);
        glDeleteFramebuffers(1, &fb_);
    }
};


class gbuffer_t : public ibuffer{
    utils::Model obj;
    glm::vec3 model_scale;
    glm::vec3 model_pos ;
    GLuint shader;
    GLuint pos_tex_;
    GLuint norm_tex_;
    GLuint diffuse_tex_;
    GLuint specular_tex_;
public:
    gbuffer_t(int width, int height) : ibuffer(width, height), obj("res/diablo.obj") {
        shader = utils::makeProg("res/gbufV.glsl", "res/gbufF.glsl");
        model_scale = glm::vec3(0.94);
        model_pos = glm:: vec3(0.0f, -0.6f, 1.0f);

        pos_tex_ = attach_new_texture(GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
        norm_tex_ = attach_new_texture( GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1);
        diffuse_tex_ = attach_new_texture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT2);
        specular_tex_ = attach_new_texture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT3);
        fin_init();

    }

    void draw(Camera& camera, int, int);
    void bind_texs(GLuint shader);
};

class lbuffer_t : public ibuffer{
    GLuint light_tex_;
public:
    GLuint shader;
    lbuffer_t(int width, int height) : ibuffer(width, height) {
        shader = utils::makeProg("res/lightV.glsl", "res/lightF.glsl");
        light_tex_ = attach_new_texture(GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
        fin_init();
    }


    void bind_texs(GLuint shader);
    void draw(Camera& camera, int width, int height, int gamma, std::vector<light_t> & lights, gbuffer_t &gbuffer, Bounds &bounds);
};

#endif
