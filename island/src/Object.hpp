// base class for drawable objects
#pragma once

#include "Shader.hpp"
#include <glm/glm.hpp>
#include <vector>

// class that stores all the array data for future use
class VectorData {
public:
    std::vector<glm::vec3> vert;
    std::vector<glm::vec3> norm;
    std::vector<glm::vec2> uv;
    std::vector<unsigned int> indices;

    VectorData(std::vector<glm::vec3> _vert, std::vector<glm::vec3> _norm, std::vector<glm::vec2> _uv, std::vector<unsigned int> _indices) {
        vert = _vert;
        norm = _norm;
        uv = _uv;
        indices = _indices;
    }
};

class Object {
protected:
    // uniform buffer data about the object position
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct ObjectData {
        glm::mat4 WorldFromModel, ModelFromWorld;
    } object;

    // arrays defining triangles for GPU
    unsigned int varrayID;              // GL vertex array object, containing:
    std::vector<glm::vec3> vert;        //   per-vertex position
    std::vector<glm::vec3> norm;        //   per-vertex normal
    std::vector<glm::vec2> uv;          //   per-vertex texture coordinate
    std::vector<unsigned int> indices;  //   3 vertex indices per triangle

    // GL texture ID(s), array for extensibility to more textures
    enum {COLOR_TEXTURE, NUM_TEXTURES};
    unsigned int textureIDs[NUM_TEXTURES];

    // GL buffer object IDs
    enum {OBJECT_UNIFORM_BUFFER, POSITION_BUFFER, NORMAL_BUFFER, UV_BUFFER, INDEX_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];

    // GL shaders
    unsigned int shaderID;      // ID for shader program
    std::vector<ShaderInfo> shaderParts;  // vertex & fragment shader info

public:
    int level;      // current level of island

public:
    // base object constructor: create buffers and textures
    Object(const char *texturePPM);

    // virtual destructor to delete any child class data
    virtual ~Object();

    // load/reload shaders
    virtual void updateShaders();

    // draw this object
    virtual void draw(class GLapp *app, double now);

    // load an image file into a texture object
    void loadPPM(const char *imagefile, unsigned int bufferID);

    // returns object containing all the needed arrays
    VectorData getVectorData();

    // updates the arrays with new data
    void setVectorData(VectorData data);
};

