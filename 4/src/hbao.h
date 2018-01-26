#ifndef HBAO_H_
#define HBAO_H_
#include "util.h"
#include <glm/gtc/random.hpp>
#include <iostream>

#define RES_RATIO 2
#define AO_RADIUS 4.0f
#define AO_DIRS 6
#define AO_SAMPLES 10
#define AO_STRENGTH 4;
#define AO_MAX_RADIUS_PIXELS 50.0
#define NOISE_RES 1000
class hbao {
    GLuint noiseTexture;
    float * noise;

    void generateNoiseTexture(int width, int height)
    {
      noise = new float[width*height*4];
      for(int y = 0; y < height; ++y)
      {
        for(int x = 0; x < width; ++x)
        {
          glm::vec2 xy = glm::circularRand(1.0f);
          float z = glm::linearRand(0.0f, 1.0f);
          float w = glm::linearRand(0.0f, 1.0f);

          int offset = 4*(y*width + x);
          noise[offset + 0] = xy[0];
          noise[offset + 1] = xy[1];
          noise[offset + 2] = z;
          noise[offset + 3] = w;
        }
      }

      GLint internalFormat = GL_RGBA16F;
      GLint format = GL_RGBA;
      GLint type = GL_FLOAT;

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, noise);
    }
    void init_noise() {
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        generateNoiseTexture(NOISE_RES, NOISE_RES);
    }
public:
    void set_texs() {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
    }
    void set_params(GLuint prog, int width, int height, float fovRad, float near, float far) {
        glm::vec2 FocalLen, InvFocalLen, UVToViewA, UVToViewB, LinMAD;
        FocalLen[0]      = 1.0f / tanf(fovRad * 0.5f) * ((float)height / (float)width);
        FocalLen[1]      = 1.0f / tanf(fovRad * 0.5f);
        InvFocalLen[0]   = 1.0f / FocalLen[0];
        InvFocalLen[1]   = 1.0f / FocalLen[1];

        UVToViewA[0] = -2.0f * InvFocalLen[0];
        UVToViewA[1] = -2.0f * InvFocalLen[1];
        UVToViewB[0] =  1.0f * InvFocalLen[0];
        UVToViewB[1] =  1.0f * InvFocalLen[1];

        LinMAD[0] = (near-far)/(2.0f*near*far);
        LinMAD[1] = (near+far)/(2.0f*near*far);


        GLuint pos;
        pos = glGetUniformLocation(prog, "FocalLen");
        glUniform2f(pos, FocalLen[0], FocalLen[1]);


        pos = glGetUniformLocation(prog, "UVToViewA");
        glUniform2fv(pos, 1,
                glm::value_ptr(glm::vec2(UVToViewA[0], UVToViewA[1])));
        pos = glGetUniformLocation(prog, "UVToViewB");
        glUniform2f(pos, UVToViewB[0], UVToViewB[1]);
        pos = glGetUniformLocation(prog, "LinMAD");
        glUniform2f(pos, LinMAD[0], LinMAD[1]);
                pos = glGetUniformLocation(prog, "AORes");

        glUniform2f(glGetUniformLocation(prog, "AORes"),
                (float)width, (float)height);
        pos = glGetUniformLocation(prog, "InvAORes");
        glUniform2f(pos, 1.0f/(float)width, 1.0f/(float)height);
        pos = glGetUniformLocation(prog, "NoiseScale");
                glUniform2f(pos, (float)width/(float)NOISE_RES, (float)height/(float)NOISE_RES);

        pos = glGetUniformLocation(prog, "R");
        glUniform1f(pos, AO_RADIUS);
        pos = glGetUniformLocation(prog, "R2");
        glUniform1f(pos, AO_RADIUS*AO_RADIUS);
        pos = glGetUniformLocation(prog, "NegInvR2");
        glUniform1f(pos, -1.0f / (AO_RADIUS*AO_RADIUS));
        pos = glGetUniformLocation(prog, "MaxRadiusPixels");
        glUniform1f(pos, AO_MAX_RADIUS_PIXELS / (float)RES_RATIO);


        pos = glGetUniformLocation(prog, "NumDirections");
        glUniform1i(pos, AO_DIRS);
        pos = glGetUniformLocation(prog, "NumSamples");
        glUniform1i(pos, AO_SAMPLES);
    }
    void init() {
        init_noise();
    }

    ~hbao() {
        glDeleteTextures(1, &noiseTexture);
        delete noise;
    }
};

#endif
