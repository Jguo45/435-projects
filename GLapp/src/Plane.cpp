// draw a simple plan3 model

#include "Plane.hpp"
#include "GLapp.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace glm;  // avoid glm:: for all glm types and functions

// load the sphere data
Plane::Plane(vec3 size, const char *texturePPM) :
    Object(texturePPM)
{
    // build texture coordinate, normal, tangent, bitangent, and vertex arrays
    uv = {vec2(0.f,0.f), vec2(1.f,0.f), vec2(0.f,1.f), vec2(1.f,1.f)};
    norm = {vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f)};
    tan = { vec3(1.f, 0.f, 0.f), vec3(1.f, 0.f, 0.f), vec3(1.f, 0.f, 0.f), vec3(1.f, 0.f, 0.f) };
    bitan = { vec3(0.f, 1.f, 0.f), vec3(0.f, 1.f, 0.f), vec3(0.f, 1.f, 0.f), vec3(0.f, 1.f, 0.f) };
    vert = {
        vec3(-0.5f*size.x, -0.5f*size.y, 0.f),
        vec3( 0.5f*size.x, -0.5f*size.y, 0.f),
        vec3(-0.5f*size.x,  0.5f*size.y, 0.f),
        vec3( 0.5f*size.x,  0.5f*size.y, 0.f)
    };

    // build index array linking sets of three vertices into triangles
    indices = {0, 1, 3,   0, 3, 2};

    // load vertex and index arrays to GPU
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), &vert[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(norm[0]), &norm[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[TAN_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, tan.size() * sizeof(tan[0]), &tan[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[BITAN_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, bitan.size() * sizeof(bitan[0]), &bitan[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(uv[0]), &uv[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

    updateShaders();
}
