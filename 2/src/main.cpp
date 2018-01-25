#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "util.h"
#include "lights.h"
#include "scene_obj.h"
#include "shadow.h"
#include "window.h"

#include <iostream>

using namespace utils;


class scene : public windowH{
   DynLamp dlamp;
   scene_floor fl;
   bunny sbunny, dbunny;
   unsigned int compose_prog;
   StaticLight slight;
   DepthShadow dshadow;
   Camera camera = Camera (glm::vec3(0.0f, 1.5f, 5.0f));

   struct {
       float pX = 0;
       float pY = 0;
       bool mouse_not_used = true;
   } controls;

   void update(float t) {
       dlamp.update(t);
       dbunny.update(t);
   }
   void draw_objs(GLuint prog) {
      fl.draw(prog);
      sbunny.draw(prog);
      dbunny.draw(prog);
   }

   void drawDepthShadowMap() {
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(dshadow.get_shader());
        slight.set_shader_depth_params(dshadow.get_shader());
        dshadow.start_draw();
        draw_objs(dshadow.get_shader());
        dshadow.finish_draw();
   }

   void drawShadowMap(window * window, glm::mat4 &pv) {
       glUseProgram(compose_prog);
        glViewport(0, 0, window->getWidth(), window->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUniformMatrix4fv(glGetUniformLocation(compose_prog, "PV"), 1, GL_FALSE, glm::value_ptr(pv));
        glUniform3fv(glGetUniformLocation(compose_prog, "viewPos"), 1, glm::value_ptr(camera.getPosition()));
        slight.set_shader_map_params(compose_prog);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dshadow.get_dmap());

        draw_objs(compose_prog);
        dlamp.draw2shadow_shader(compose_prog);
   }
public:

   int draw(window * window) override {
       update(glfwGetTime());

       drawDepthShadowMap();

       glm::mat4 pv = camera.getPV(window->getWidth(), window->getHeight());

       drawShadowMap(window, pv);

       dlamp.draw2pointlight_shader(pv);

       return 0;
   }

   void before_loop(window*) override {
       compose_prog = makeProg("res/composeV.glsl", "res/composeF.glsl");
        dlamp.init();
        dshadow.init();

        glUseProgram(compose_prog);
        glUniform1i(glGetUniformLocation(compose_prog, "shadowMap"), 1);
   }
   void on_key(window *, int key) override {
       static float k = 0.05;
       static float slight_delta = 0.2;
       switch (key) {
       case GLFW_KEY_W:
           camera.move(Camera::F, k);
           break;
       case GLFW_KEY_S:
           camera.move(Camera::B, k);
          break;
       case GLFW_KEY_A:
           camera.move(Camera::L, k);
             break;
       case GLFW_KEY_D:
           camera.move(Camera::R, k);
             break;
       case GLFW_KEY_B:
                  slight.addX(slight_delta);
                    break;
       case GLFW_KEY_N:
                         slight.addX(-slight_delta);
                           break;
       case GLFW_KEY_G:
                  slight.addY(slight_delta);
                    break;
       case GLFW_KEY_H:
                         slight.addY(-slight_delta);
                           break;

       }
   }
   void on_mouse(window*, double xpos, double ypos) override {
       if (controls.mouse_not_used)
       {
           controls.pX = (xpos);
           controls.pY = (ypos);
           controls.mouse_not_used = false;
       }

       float xoff = (xpos - controls.pX);
       float yoff = (controls.pY - ypos);
       camera.move2off(xoff, yoff);
       controls.pX = (xpos);
       controls.pY = (ypos);
   }
   void on_scroll(window*, double xoffset, double yoffset) override { }
   void framebuffer_size_callback(window*, int width, int height) override {   }
};

int main()
{
    glfwInit();


    window win(800, 600, "shadow");
    scene sc;
    win.loop(&sc);

    glfwTerminate();

    return 0;
}

