#include "window.h"


static void key_callback(GLFWwindow* glwin, int key, int scancode, int action, int mods) {
    window * win = (window*)glfwGetWindowUserPointer(glwin);
    win->getHandler()->on_key(win, key);
}

static void mouse_callback(GLFWwindow* glwin, double xpos, double ypos) {
    window * win = (window*)glfwGetWindowUserPointer(glwin);
    win->getHandler()->on_mouse(win, xpos, ypos);
}

static  void scroll_callback(GLFWwindow* glwin, double xoff, double yoff) {
    window * win = (window*)glfwGetWindowUserPointer(glwin);
    win->getHandler()->on_scroll(win, xoff, yoff);
}

static void framebuffer_size_callback(GLFWwindow* glwin, int width, int height) {
    window * win = (window*)glfwGetWindowUserPointer(glwin);
    win->getHandler()->framebuffer_size_callback(win, width, height);
    /*
    win->width = width;
    win->height = height;
    */
}


window::window(int width, int height, std::string title) : handler(nullptr), width(width), height(height){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    win = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!win) {
        throw std::exception();
    }

    glfwMakeContextCurrent(win);
    glfwSetCursorPosCallback(win, mouse_callback);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
    glfwSetScrollCallback(win, scroll_callback);
    glfwSetKeyCallback(win, key_callback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
}
