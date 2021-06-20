#version 410 core
// simple object fragment shader

// per-frame data
layout(std140)                  // standard layout
uniform SceneData {             // like a class name
    mat4 ProjFromWorld, WorldFromProj;
    vec4 LightDir;
    float Col;
    float AO;
    float Gloss;
    float Norm;
};

// shader settings
uniform sampler2D ColorTexture;

uniform sampler2D PropTexture;

uniform sampler2D NormTexture;

// input from vertex shader
in vec2 texcoord;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec4 position;
in float props;

// output to frame buffer
out vec4 fragColor;

void main() {
    // unpack normals
    vec3 nmap = texture(NormTexture, texcoord).xyz;
    vec3 Ntang = 2 * nmap - 1;

    mat3 TBN = mat3(tangent, bitangent, normal);    // TBN matrix
    vec3 N = TBN * Ntang;   // normal based on normal texture

    if (props == 0)         // check if property file exists for this texture
        N = normal;

    N = normalize(N);

    if (Norm == 0)          // toggle normal map
        N = normalize(normal);

    vec3 L = normalize(LightDir.xyz);       // light direction
    float I = max(0., dot(N,L));            // diffuse lighting
    
    I = I + (LightDir.a * texture(PropTexture, texcoord).b);    // scales ambient light by property file

    if (AO == 0) {          // toggle ambient occlusion map
        I = max(0., dot(N,L)); 
        I = min(1., I + LightDir.a);
    }

    vec4 eyeProj = vec4(0, 0, -1, 0);
    vec4 eyeWorld = WorldFromProj * eyeProj;
    vec3 V = eyeWorld.xyz/eyeWorld.w - position.xyz/position.w;
    V = normalize(V);
    vec3 color = I * texture(ColorTexture, texcoord).rgb;

    if (Col == 0) {         // toggle color map
        color = I * vec3(.7);
    }

    // specular reflection
    float fresnel = 0.04 + 0.96 * pow(1. - dot(N, V), 5);
    float specPow = pow(2, 12 * texture(PropTexture, texcoord).g);
    if (Gloss == 0) {       // toggle gloss map
        specPow = 64.;
    }
    vec3 H = normalize(V + L);
    float spec = pow(max(dot(N, H), 0.), specPow) * ((specPow + 1) / 2);
    color = mix(color, vec3(spec), fresnel);

    // final color
    fragColor = vec4(color, 1);
}
