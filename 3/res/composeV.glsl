#version 450 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv;

out vec2 vs_uv;

void main()
{
    gl_Position = vec4(in_pos, 1);
    vs_uv = in_uv;
}
