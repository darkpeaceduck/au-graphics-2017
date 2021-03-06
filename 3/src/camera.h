#ifndef CAMERA_H
#define CAMERA_H

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


class Camera {
    glm::vec3 _position;
       glm::vec3 _front;
       glm::vec3 _up;
       glm::vec3 _right;
       glm::vec3 _worldUp;
       float xangle;
       float yangle;
       float _movementSpeed;
       float _mouseSensitivity;

    void update() {
            glm::vec3 front;
            double xcos = cos(glm::radians(xangle));
            double ycos = cos(glm::radians(yangle));
            double xsin = sin(glm::radians(xangle));
            double ysin = sin(glm::radians(yangle));
            front = glm::vec3(xcos * ycos, ysin, xsin * ycos);
            _front = glm::normalize(front);
            _right = glm::normalize(glm::cross(_front, _worldUp));
            _up    = glm::normalize(glm::cross(_right, _front));
    }
public:
    enum Dir {
        F,
        B,
        L,
        R
    };


    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f)
            : _front(glm::vec3(0.0f, 0.0f, -1.0f))
            , _mouseSensitivity(0.1f)
            , _position(position)
            , _worldUp(up)
            , xangle(-90.0f)
            , yangle(0.0f) {
        update();
    }

    glm::vec3 getPosition() {
        return _position;
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(_position, _position + _front, _up);
    }

    glm::mat4 getPerspective (int width, int height) {
        return glm::perspective(glm::radians(45.0f), (float)width/ (float)height, 0.1f, 100.0f);
    }

    glm::mat4 getPV(int width, int height) {
        return getPerspective(width, height) * getViewMatrix();
    }

    void move2off(float xoff, float yoff) {
       xoff *= _mouseSensitivity;
       yoff *= _mouseSensitivity;

       xangle   += xoff;
       yangle += yoff;

       update();
   }

    void move(Dir direction, float k) {
        float velocity = 2.5f * k;
        switch (direction) {
            case F:
                _position += _front * velocity;
                break;
            case B:
                _position -= _front * velocity;
                break;
            case L:
                _position -= _right * velocity;
                break;
            case R:
                _position += _right * velocity;
                break;
            default:
                break;
        }
    }


};

#endif
