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
#include "hbao.h"

#include <iostream>

using namespace utils;


class scene : public windowH{
   DynLamp dlamp;
   scene_floor fl;
   bunny sbunny;
   GLuint compose_prog;
   StaticLight slight;
   DepthShadow dshadow;
   Camera camera = Camera(glm::vec3(-0.109187f, 0.621613f, 0.001249),
           glm::vec3(0.0f, 1.0f, 0.0f), -176.600983f, -33.700012f);
   hbao hb;
   int render_mode = 0;

   struct {
       float pX = 0;
       float pY = 0;
       bool mouse_not_used = true;
   } controls;

   void update(float t) {
       dlamp.update(t);
   }
   void draw_objs(GLuint prog) {
      fl.draw(prog);
      sbunny.draw(prog);
   }

   void drawDepthShadowMap() {
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(dshadow.get_shader());
        slight.set_shader_depth_params(dshadow.get_shader());
        dshadow.start_draw();
        draw_objs(dshadow.get_shader());
        dshadow.finish_draw();
   }

   void drawDepthShadowMapCam(int width, int height) {
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
           glUseProgram(dshadow.get_shader());
           glUniformMatrix4fv(glGetUniformLocation(dshadow.get_shader(), "lightM"),
                      1, GL_FALSE, glm::value_ptr(camera.getPV(width, height)));
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
        glUniform1i(glGetUniformLocation(compose_prog, "mode"), render_mode);
        slight.set_shader_map_params(compose_prog);
        hb.set_params(compose_prog,
                window->getWidth() , window->getHeight(),
                slight.getFOVRad(), slight.nearPlane,
                slight.farPlane);
//        hb.set_params(compose_prog,
//                window->getWidth() , window->getHeight(),
//                camera.getFov(), slight,
//                slight.farPlane);


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dshadow.get_dmap());
        hb.set_texs();


        dlamp.draw2shadow_shader(compose_prog);
        draw_objs(compose_prog);
   }
   void init_hbao(window* win) {
       hb.init();
   }
   void change_mode() {
       render_mode = (render_mode + 1) % 2;
   }
public:

   int draw(window * window) override {
       slight.assignCamera(camera, window->getWidth(), window->getHeight());
       update(glfwGetTime());

       drawDepthShadowMap();

       glm::mat4 pv = camera.getPV(window->getWidth(), window->getHeight());

       drawShadowMap(window, pv);
       return 0;
   }


   void before_loop(window* win) override {
       compose_prog = makeProg("res/composeV.glsl", "res/composeF.glsl");
        dlamp.init();
        dshadow.init();

        glUseProgram(compose_prog);
        glUniform1i(glGetUniformLocation(compose_prog, "shadowMap"), 1);

        init_hbao(win);
   }
   void on_key(window *, int key, int action) override {
       static float k = 0.05;
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
       case GLFW_KEY_M:
           if (action == GLFW_PRESS)
               change_mode();
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

       glm::vec3 pos = camera.getPosition();
//       printf("%f %f %f\n", pos.x ,pos.y, pos.z);
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

