#version 450 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;

out vec3 vs_norm;
out vec3 vs_pos;

uniform mat4 PV;
uniform mat4 model;


void main()
{
    gl_Position = PV * model * vec4(in_pos, 1);
    vs_pos = vec3(model * vec4(in_pos, 1));
    vs_norm = mat3(transpose(inverse(model))) * in_norm;
}
