#version 450 core

in vec3 vs_pos;
in vec3 vs_norm;

layout(location = 0) out vec3 out_pos;
layout(location = 1) out vec3 out_norm;
layout(location = 2) out vec3 out_diffuse;
layout(location = 3) out vec4 out_specular;

void main()
{
    out_pos = vs_pos;
    out_norm = vs_norm;
    out_diffuse = vec3(0.5, 0.5, 0.5);
    out_specular = vec4(0.1, 0.1, 0.1, 32.0);
}
