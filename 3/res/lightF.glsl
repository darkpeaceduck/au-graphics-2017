#version 450 core

uniform sampler2D gbuffer_pos;
uniform sampler2D gbuffer_norm;
uniform sampler2D gbuffer_diffuse;
uniform sampler2D gbuffer_specular;

out vec4 out_light;

uniform struct {
    vec3 position;
    float size;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} light;
uniform vec2 window;
uniform vec3 camera;
uniform float gamma;

float fading_kof(float dist, float soft, float hard) {
    float a = 0.1;
    float b = 2;
    float c = 10;
    float d = min(dist, soft);
    float k = 1.0 / (a + b * d + c * d * d);
    if (soft == d) {
        float diff = hard - soft;
        k *= 1.0 - clamp(dist - soft, 0.0, diff) / diff;
    }
    return k;
}
void main()
{
    vec2 uv = gl_FragCoord.xy / window;
    vec3 pos = texture(gbuffer_pos, uv).xyz;
    vec3 norm = normalize(texture(gbuffer_norm, uv).xyz);
    vec3 kd = texture(gbuffer_diffuse, uv).rgb;
    vec3 ka = kd;
    vec4 ks = texture(gbuffer_specular, uv).rgba;

    //apply gamma
    kd = pow(kd, vec3(gamma));
    ka = pow(ka, vec3(gamma));
    ks.rgb = pow(ks.rgb, vec3(gamma));
    vec3 ambient = pow(light.ambient, vec3(gamma));
    vec3 diffuse = pow(light.diffuse, vec3(gamma));
    vec3 specular = pow(light.specular, vec3(gamma));

    //phong
    ambient *= ka;
    vec3 lightDir = normalize(light.position - pos);
    diffuse *= max(dot(norm, lightDir), 0.0) * kd;
    specular *= pow(max(dot(normalize(camera - pos), reflect(-lightDir, norm)), 0.0), 16.0) * ks.rgb;

    // fading kof
    float k = fading_kof(distance(light.position, pos) / light.size, 0.75, 0.95);
    out_light = vec4(ambient + diffuse * k + specular * k, 1);
}
