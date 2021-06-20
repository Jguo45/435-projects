#version 410 core
// simple object vertex shader

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

// per-object data
layout(std140)
uniform ObjectData {
    mat4 WorldFromModel, ModelFromWorld;
    float Props;
};

// per-vertex input
in vec2 vUV;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;

// output to fragment shader (view space)
out vec2 texcoord;
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;
out vec4 position;
out float props;

void main() {
    texcoord = vUV;
    props = Props;
    position = WorldFromModel * vec4(vPosition, 1);
    normal = normalize(vNormal * mat3(ModelFromWorld));
    tangent = normalize(mat3(WorldFromModel) * vTangent);
    bitangent = -normalize(mat3(WorldFromModel) * vBiTangent);
    gl_Position = ProjFromWorld * position;
}
