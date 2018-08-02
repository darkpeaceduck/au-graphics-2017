#version 450 core

const float PI = 3.14159265;

uniform sampler2D shadowMap;
//noise tex for hbao
uniform sampler2D noise;
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


// for hbao
uniform vec2 FocalLen;
uniform vec2 UVToViewA;
uniform vec2 UVToViewB;
uniform vec2 LinMAD;
uniform vec2 AORes;
uniform vec2 InvAORes ;
uniform vec2 NoiseScale ;
uniform float AOStrength = 1.9;
uniform float R;
uniform float R2;
uniform float NegInvR2 ;
uniform float TanBias = tan(30.0 * PI / 180.0);
uniform float MaxRadiusPixels ;
uniform int NumDirections ;
uniform int NumSamples ;

// mode
uniform int mode;

vec2 TexCoord;

in vec3 fpos;
in vec3 Normal;
in vec4 fpos_light;

out vec4 out_color;


void produceTC() {
    float mkof = 0.5;
    vec3 fcords = mkof + mkof * (fpos_light.xyz / fpos_light.w);
    TexCoord = fcords.xy;
}



float ViewSpaceZFromDepth(float d)
{
    // [0,1] -> [-1,1] clip space
    d = d * 2.0 - 1.0;

    // Get view space Z
    return -1.0 / (LinMAD.x * d + LinMAD.y);
}

vec3 UVToViewSpace(vec2 uv, float z)
{
    uv = UVToViewA * uv + UVToViewB;
    return vec3(uv * z, z);
}

vec3 GetViewPos(vec2 uv)
{
    float z = ViewSpaceZFromDepth(texture(shadowMap, uv).r);
    return UVToViewSpace(uv, z);
}

vec3 GetViewPosPoint(ivec2 uv)
{
    ivec2 coord = ivec2(gl_FragCoord.xy) + uv;
    float z = texelFetch(shadowMap, coord, 0).r;
    return UVToViewSpace(uv, z);
}


float TanToSin(float x)
{
    return x * inversesqrt(x*x + 1.0);
}

float InvLength(vec2 V)
{
    return inversesqrt(dot(V,V));
}

float Tangent(vec3 V)
{
    return V.z * InvLength(V.xy);
}

float BiasedTangent(vec3 V)
{
    return V.z * InvLength(V.xy) + TanBias;
}

float Tangent(vec3 P, vec3 S)
{
    return -(P.z - S.z) * InvLength(S.xy - P.xy);
}

float Length2(vec3 V)
{
    return dot(V,V);
}

vec3 MinDiff(vec3 P, vec3 Pr, vec3 Pl)
{
    vec3 V1 = Pr - P;
    vec3 V2 = P - Pl;
    return (Length2(V1) < Length2(V2)) ? V1 : V2;
}

vec2 SnapUVOffset(vec2 uv)
{
    return round(uv * AORes) * InvAORes;
}

float Falloff(float d2)
{
    return d2 * NegInvR2 + 1.0f;
}

float HorizonOcclusion( vec2 deltaUV,
                        vec3 P,
                        vec3 dPdu,
                        vec3 dPdv,
                        float randstep,
                        float numSamples)
{
    float ao = 0;

    // Offset the first coord with some noise
    vec2 uv = TexCoord + SnapUVOffset(randstep*deltaUV);
    deltaUV = SnapUVOffset( deltaUV );

    // Calculate the tangent vector
    vec3 T = deltaUV.x * dPdu + deltaUV.y * dPdv;

    // Get the angle of the tangent vector from the viewspace axis
    float tanH = BiasedTangent(T);
    float sinH = TanToSin(tanH);

    float tanS;
    float d2;
    vec3 S;

    // Sample to find the maximum angle
    for(float s = 1; s <= numSamples; ++s)
    {
        uv += deltaUV;
        S = GetViewPos(uv);
        tanS = Tangent(P, S);
        d2 = Length2(S - P);

        // Is the sample within the radius and the angle greater?
        if(d2 < R2 && tanS > tanH)
        {
            float sinS = TanToSin(tanS);
            // Apply falloff based on the distance
            ao += Falloff(d2) * (sinS - sinH);

            tanH = tanS;
            sinH = sinS;

        }
    }

    return ao;
}


vec2 RotateDirections(vec2 Dir, vec2 CosSin)
{
    return vec2(Dir.x*CosSin.x - Dir.y*CosSin.y,
                  Dir.x*CosSin.y + Dir.y*CosSin.x);
}

void ComputeSteps(inout vec2 stepSizeUv, inout float numSteps, float rayRadiusPix, float rand)
{
    // Avoid oversampling if numSteps is greater than the kernel radius in pixels
    numSteps = min(NumSamples, rayRadiusPix);

    // Divide by Ns+1 so that the farthest samples are not fully attenuated
    float stepSizePix = rayRadiusPix / (numSteps + 1);

    // Clamp numSteps if it is greater than the max kernel footprint
    float maxNumSteps = MaxRadiusPixels / stepSizePix;
    if (maxNumSteps < numSteps)
    {
        // Use dithering to avoid AO discontinuities
        numSteps = floor(maxNumSteps + rand);
        numSteps = max(numSteps, 1);
        stepSizePix = MaxRadiusPixels / numSteps;
    }

    // Step size in uv space
    stepSizeUv = stepSizePix * InvAORes;
}


float hbao(void)
{

    produceTC();


    float numDirections = NumDirections;

    vec3 P, Pr, Pl, Pt, Pb;
    P   = GetViewPos(TexCoord);

    // Sample neighboring pixels


    Pr  = GetViewPos(TexCoord + vec2( InvAORes.x, 0));
    Pl  = GetViewPos(TexCoord + vec2(-InvAORes.x, 0));
    Pt  = GetViewPos(TexCoord + vec2( 0, InvAORes.y));
    Pb  = GetViewPos(TexCoord + vec2( 0,-InvAORes.y));

    // Calculate tangent basis vectors using the minimu difference
    vec3 dPdu = MinDiff(P, Pr, Pl);
    vec3 dPdv = MinDiff(P, Pt, Pb) * (AORes.y * InvAORes.x);

    // Get the random samples from the noise texture
//  vec3 random = texture(noise, TexCoord.xy * NoiseScale).rgb;
    vec3 random = texture(noise, TexCoord.xy).rgb;

    // Calculate the projected size of the hemisphere
//    vec2 rayRadiusUV = 0.5 * R * FocalLen / -P.z;
    vec2 rayRadiusUV = 0.5 * R * FocalLen / -P.z;
    float rayRadiusPix = rayRadiusUV.x * AORes.x;

    float ao = 1.0;



    // Make sure the radius of the evaluated hemisphere is more than a pixel
    if(rayRadiusPix > 1.0)
    {
        ao = 0.0;
        float numSteps;
        vec2 stepSizeUV;

        // Compute the number of steps
        ComputeSteps(stepSizeUV, numSteps, rayRadiusPix, random.z);

        float alpha = 2.0 * PI / numDirections;

        // Calculate the horizon occlusion of each direction
        for(float d = 0; d < numDirections; ++d)
        {
            float theta = alpha * d;

            // Apply noise to the direction
            vec2 dir = RotateDirections(random.xy, vec2(cos(theta), sin(theta)));
            vec2 deltaUV = dir * stepSizeUV;

            // Sample the pixels along the direction
            ao += HorizonOcclusion( deltaUV,
                                    P,
                                    dPdu,
                                    dPdv,
                                    random.z,
                                    numSteps);
        }
        ao = 1.0 - ao / numDirections * AOStrength;
    }
    return ao;
}


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
    for(int x = -1; x <= 1; ++x) {
       for(int y = -1; y <= 1; ++y) {
           vec2 at = fcords.xy + vec2(x, y) * size;
           float pcf = texture(shadowMap, at).r;
           bool compare = fcords.z > bias + pcf;
           shadow += compare ? 0.7 : 0.0;
       }
    }
    shadow /= 25.0;
    float ao = hbao();

    if (mode == 0)
        out_color = vec4(ambient + (1.0 - shadow * ao) * (diffuse + specular) , 1.0);
    else
        out_color = vec4(vec3(ao), 1);
}
