#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 fpos;
out vec3 Normal;
out vec4 fpos_light;

uniform mat4 PV;
uniform mat4 model;
uniform mat4 lightM;

void main() {
    fpos = vec3(model * vec4(aPos, 1.0));
    Normal = transpose(inverse(mat3(model))) * aNormal;
    fpos_light = lightM * vec4(fpos, 1.0);
    gl_Position = PV * model * vec4(aPos, 1.0);
}
