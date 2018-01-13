#ifndef SHADOW_H_
#define SHADOW_H_

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"

class DepthShadow {
    GLuint fb;
    GLuint dmap;
    GLuint shader;
    static constexpr GLsizei width = 1024;
    static constexpr GLsizei height = 1024;
public:
    void load_shader() {
        shader = utils::makeProg("res/depthMapV.glsl", "res/depthMapF.glsl");
    }
    GLuint get_shader() {
        return shader;
    }
    void init() {
        load_shader();
        glGenFramebuffers(1, &fb);
        glGenTextures(1, &dmap);
        glBindTexture(GL_TEXTURE_2D, dmap);
        glTexImage2D(GL_TEXTURE_2D, 0,
                GL_DEPTH_COMPONENT, width, height, 0,
                GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dmap, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


    }
    void start_draw() {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
    }
    void finish_draw() {
        glDisable(GL_POLYGON_OFFSET_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    GLuint get_dmap() {
        return dmap;
    }
};

#endif
