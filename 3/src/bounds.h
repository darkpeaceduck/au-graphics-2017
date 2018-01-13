#ifndef BOUNDS_H_
#define BOUNDS_H_
#include "util.h"
struct Bounds {
    Bounds();
    ~Bounds();
    void draw();
private:
    GLuint vao_, vbo_;
};

#endif
