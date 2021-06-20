#version 410 core
// simple object vertex shader

// per-frame data
layout(std140)                  // standard layout
uniform SceneData {             // like a class name
    mat4 ProjFromWorld, WorldFromProj;
    vec4 LightDir;
};

// per-object data
layout(std140)
uniform ObjectData {
    mat4 WorldFromModel, ModelFromWorld;
};

// per-vertex input
in vec2 vUV;
in vec3 vPosition;
in vec3 vNormal;

// output to fragment shader (view space)
out vec2 texcoord;
out vec3 normal;
out vec4 position;

void main() {
    texcoord = vUV;
    position = WorldFromModel * vec4(vPosition, 1);
    normal = normalize(vNormal * mat3(ModelFromWorld));
    gl_Position = ProjFromWorld * position;
}
