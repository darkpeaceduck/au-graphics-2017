#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/vec3.hpp>
#include "buffers.h"
#include "light.h"
#include "window.h"
class scene : public windowH{
   std::vector<light_t> lights;
   Camera camera = Camera (glm::vec3(0.0f, -0.5f, 5.0f));
   gbuffer_t gbuffer;
   lbuffer_t lbuffer;
   Bounds quad;
   int mode;
   GLuint compose_shader;
   float gamma = 2.2f;

   struct {
       float pX = 0;
       float pY = 0;
       bool mouse_not_used = true;
   } controls;

   void update(float t) {
       for (size_t i = 0; i < lights.size(); ++i) {
           lights[i].update_position(t);
       }
   }

   void change_mode() {
       mode = (mode + 1) % 5;
   }

   void draw_gbuffer(window * window) {
       gbuffer.draw(camera, window->getWidth(), window->getHeight());
   }

   void  draw_lbuffer(window * window) {
       lbuffer.draw(camera, window->getWidth(), window->getHeight(), gamma, lights, gbuffer,  quad);
   }
   void draw_combined() {
       glBindFramebuffer(GL_FRAMEBUFFER, 0);
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       glDisable(GL_DEPTH_TEST);
       glDisable(GL_BLEND);
       glUseProgram(compose_shader);
       gbuffer.bind_texs(compose_shader);
       lbuffer.bind_texs(compose_shader);
       glUniform1i(glGetUniformLocation(compose_shader, "mode"), mode);
       glUniform1f(glGetUniformLocation(compose_shader, "gamma"), gamma);
       quad.draw();
   }


   void add_lights(int add, float t) {
       lights.resize(std::max(0, ((int)lights.size()) + add));
       for (size_t i = 0; i < lights.size(); ++i) {
           lights[i] = light_t(t);
       }
   }
public:
   scene() : gbuffer(800, 600), lbuffer(800, 600), mode(0) {}

   int draw(window * window) override {
      update(glfwGetTime());
      draw_gbuffer(window);
      draw_lbuffer(window);
      draw_combined();
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
      return 0;
   }

   void before_loop(window*) override {
       compose_shader = utils::makeProg("res/composeV.glsl", "res/composeF.glsl");
       add_lights(+20, 0);
   }
   void on_key(window *win, int key, int action) override {
       static float k = 0.05;
       switch (key) {
       case GLFW_KEY_Z:
          add_lights(+ 10, win->getT());
          break;
      case GLFW_KEY_X:
          add_lights(-10 , win->getT());
          break;
      case GLFW_KEY_G:
          gamma += 0.2f;
          break;
      case GLFW_KEY_H:
          gamma -= 0.2f;
          break;
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
       case GLFW_KEY_M: {
                   if (action == GLFW_PRESS) {
                       change_mode();
                   }
                   break;

               }

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


int main(int argc, char ** argv) {
    glfwInit();


    window win(800, 600, "deferred");
    scene sc;
    win.loop(&sc);

    glfwTerminate();

    return 0;
}
