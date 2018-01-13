#include "light.h"

#include <iostream>


void light_t::update_position(float t_) {
    float t = t_ - dt;
    float phi = p * t;
    float b = h / (2 * M_PI);
    float x = a * cos (phi);
    float y = a * sin (phi);
    float z = b * phi;

    glm::vec3 rposition = c + glm::vec3(x, y, z);

    if (z >= 4 * h && p > 0) {
        c = glm::vec3(0, 0, rposition.z);
        p = -p;
        dt = t_;
    }
    if (z <= 4*  -h && p < 0) {
        c = glm::vec3(0, 0, rposition.z);
        p = -p;
        dt = t_;
    }
    position = glm::vec3(rposition.x, rposition.z, rposition.y);
}


void light_t::draw(GLuint shader,Camera& camera, int window_width, int window_height) {
    glm::mat4 mvp = camera.getPV(window_width, window_height) * (translate(glm::mat4(), position));

   glUniformMatrix4fv(glGetUniformLocation(shader, "mvp"), 1, GL_FALSE, value_ptr(mvp));

   glUniform3fv(glGetUniformLocation(shader, "light.position"), 1, value_ptr(position));
   glUniform1f(glGetUniformLocation(shader, "light.size"), size);
   glUniform3fv(glGetUniformLocation(shader, "light.ambient"), 1, value_ptr(ambient));
   glUniform3fv(glGetUniformLocation(shader, "light.diffuse"), 1, value_ptr(diffuse));
   glUniform3fv(glGetUniformLocation(shader, "light.specular"), 1, value_ptr(specular));
}
