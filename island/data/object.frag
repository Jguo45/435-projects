#version 410 core
// simple object fragment shader

// per-frame data
layout(std140)                  // standard layout
uniform SceneData {             // like a class name
    mat4 ProjFromWorld, WorldFromProj;
    vec4 LightDir;
};

// shader settings
uniform sampler2D ColorTexture;

// input from vertex shader
in vec2 texcoord;
in vec3 normal;
in vec4 position;

// output to frame buffer
out vec4 fragColor;

void main() {
    vec3 N = normalize(normal);             // surface normal
    vec3 L = normalize(LightDir.xyz);       // light direction
    float I = max(0., dot(N,L));            // diffuse lighting
    I = min(1., I + LightDir.a);            // add in ambient

    // color from texture and diffuse
    vec3 color = I * texture(ColorTexture, texcoord).rgb;

    // final color
    fragColor = vec4(color, 1);
}
