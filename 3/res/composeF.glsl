#version 450 core

in vec2 vs_uv;

uniform sampler2D gbuffer_pos;
uniform sampler2D gbuffer_norm;
uniform sampler2D gbuffer_diffuse;
uniform sampler2D gbuffer_specular;
uniform sampler2D lbuffer;

uniform int mode;
uniform float gamma;

out vec4 out_color;

void main()
{
    vec3 pos = texture(gbuffer_pos, vs_uv).xyz;
    vec3 norm = texture(gbuffer_norm, vs_uv).xyz;
    vec3 diffuse = texture(gbuffer_diffuse, vs_uv).rgb;
    vec4 specular = texture(gbuffer_specular, vs_uv).rgba;
    vec3 color = texture(lbuffer, vs_uv).rgb;

    color = pow(color, vec3(1 / gamma));

    if (mode == 0) {
        out_color = vec4(color, 1);
    } else if (mode == 1) {
        out_color = vec4(norm, 1);
    } else if (mode == 2) {
        out_color = vec4(pos, 1);
    } else if (mode == 3) {
        out_color = vec4(diffuse, 1);
    } else if (mode == 4) {
        out_color = specular;
    }
}
