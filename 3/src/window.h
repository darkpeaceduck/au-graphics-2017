#ifndef WINDOW_H_
#define WINDOW_H_
#include <string>
#include <GLFW/glfw3.h>

class window;
class windowH {
public:
    virtual int draw(window *) = 0;
    virtual void on_key(window *, int, int) = 0;
    virtual void on_mouse(window*, double, double) = 0;
    virtual void on_scroll(window*, double, double) = 0;
    virtual void framebuffer_size_callback(window*, int, int) = 0;
    virtual void before_loop(window*) = 0;
    virtual ~windowH() {};
};


class window {
    GLFWwindow* win;
    windowH * handler;
    int width, height;
    float t;
public:
    window(int width, int height, std::string title);
    void loop(windowH * handler) {
        this->handler = handler;
        glfwSetWindowUserPointer(win, this);


        handler->before_loop(this);
        while (!glfwWindowShouldClose(win))
        {
            t = glfwGetTime();
            if (handler->draw(this)) {
                glfwSetWindowShouldClose(win, GLFW_TRUE);
            }
            glfwSwapBuffers(win);
            glfwPollEvents();
        }
    }
    ~window() {
        if (win) {
            glfwDestroyWindow(win);
        }
    }
    windowH * getHandler() { return handler; }
    int getWidth() { return width; };
    int getHeight() { return height; };
    float getT() { return t;}
};


#endif
