#version 450 core
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform struct {
    float shin;
    vec3 amb;
    vec3 diff;
    vec3 spec;
} material;
uniform struct {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} light;

in vec3 fpos;
in vec3 Normal;
in vec4 fpos_light;

out vec4 out_color;

void main() {
    // phong
    vec3 ambient = light.ambient * material.amb;
    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(light.position - fpos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diff);

    vec3 viewDir = normalize(viewPos - fpos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shin);
    vec3 specular =  light.specular * (spec * material.spec);
    float mkof = 0.5;
    vec3 fcords = mkof + mkof * (fpos_light.xyz / fpos_light.w);
    float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);


    // soft shadow mapping + pcf
    // http://codeflow.org/entries/2013/feb/15/soft-shadow-mapping/#percentage-closer-filtering-pcf
    float shadow = 0.0;
    vec2 size = 1.0 / textureSize(shadowMap, 0);
    for(int x = -2; x <= 2; ++x) {
       for(int y = -2; y <= 2; ++y) {
           vec2 at = fcords.xy + vec2(x, y) * size;
           float pcf = texture(shadowMap, at).r;
           bool compare = fcords.z > bias + pcf;
           shadow += compare ? 0.35 : 0.0;
       }
    }
    shadow /= 18.0;

    out_color = vec4(ambient + (1.0 - shadow) * (diffuse + specular) , 1.0);
}
