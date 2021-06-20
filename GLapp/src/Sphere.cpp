// draw a simple sphere model

#include "Sphere.hpp"
#include "GLapp.hpp"
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace glm;  // avoid glm:: for all glm types and functions

#ifndef F_PI
#define F_PI 3.1415926f
#endif

// load the sphere data
Sphere::Sphere(int w, int h, vec3 size, const char* texturePPM, const char* propPPM, const char* normPPM) :
    Object(texturePPM, propPPM, normPPM)
{
    position = vec3(0, 0, 100);
    // build vertex, normal and texture coordinate arrays
    // * x & y are longitude and latitude grid positions
    for(unsigned int y=0;  y <= h;  ++y) {
        for(unsigned int x=0;  x <= w;  ++x) {
            // Texture coordinates scaled from x and y. Be sure to cast before division!
            float u = float(x)/float(w), v = float(y) / float(h);

            // Calculate tangent and bitangent vectors
            tan.push_back(vec3(-sinf(2 * F_PI * u), cosf(2 * F_PI * u), 0));
            bitan.push_back(vec3(cosf(F_PI * v) * cosf(2 * F_PI * u), cosf(F_PI * v) * sinf(2 * F_PI * u), -sinf(F_PI * v)));

            uv.push_back(vec2(u,v));

            // normal for sphere is normalized position in spherica coordinates
            float cx = cosf(2.f * F_PI * u), sx = sinf(2.f * F_PI * u);
            float cy = cosf(F_PI * v), sy = sinf(F_PI * v);
            vec3 N = vec3(cx * sy, sx * sy, cy);
            norm.push_back(N);

            // 3d vertex location scaled by sphere size
            vert.push_back(size * N);
        }
    }

    // build index array linking sets of three vertices into triangles
    // two triangles per square in the grid. Each vertex index is
    // essentially its unfolded grid array position. Be careful that
    // each triangle ends up in counter-clockwise order
    for(unsigned int y=0; y<h; ++y) {
        for(unsigned int x=0; x<w; ++x) {
            indices.push_back((w+1)* y    + x);
            indices.push_back((w+1)* y    + x+1);
            indices.push_back((w+1)*(y+1) + x+1);

            indices.push_back((w+1)* y    + x);
            indices.push_back((w+1)*(y+1) + x+1);
            indices.push_back((w+1)*(y+1) + x);
        }
    }

    // load vertex and index array to GPU
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

void Sphere::updateShaders()
{
    loadShaders(shaderID, shaderParts);
    glUseProgram(shaderID);

    // Bind uniform block #s to their shader names. Indices should match glBindBufferBase in draw
    glUniformBlockBinding(shaderID, glGetUniformBlockIndex(shaderID, "SceneData"), 0);
    glUniformBlockBinding(shaderID, glGetUniformBlockIndex(shaderID, "ObjectData"), 1);

    // Map shader name for texture. Index# should match GL_TEXTURE# used in draw
    glUniform1i(glGetUniformLocation(shaderID, "ColorTexture"), 0);

    // Map shader name for properties
    glUniform1i(glGetUniformLocation(shaderID, "PropTexture"), 1);

    // Map shader name for normals
    glUniform1i(glGetUniformLocation(shaderID, "NormTexture"), 2);

    // bind attribute arrays
    glBindVertexArray(varrayID);

    GLint positionAttrib = glGetAttribLocation(shaderID, "vPosition");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);

    GLint normalAttrib = glGetAttribLocation(shaderID, "vNormal");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalAttrib);

    GLint tangentAttrib = glGetAttribLocation(shaderID, "vTangent");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[TAN_BUFFER]);
    glVertexAttribPointer(tangentAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(tangentAttrib);

    GLint bitangentAttrib = glGetAttribLocation(shaderID, "vBiTangent");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[BITAN_BUFFER]);
    glVertexAttribPointer(bitangentAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(bitangentAttrib);

    GLint uvAttrib = glGetAttribLocation(shaderID, "vUV");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(uvAttrib);
}

//
// this is called every time the sphere needs to be redrawn 
//
void Sphere::draw(GLapp *app, double now)
{
    // update model position
    object.WorldFromModel = translate(mat4(1), position)
                            * rotate(mat4(1), -app->pan, vec3(0, 0, 1));

    object.ModelFromWorld = inverse(object.WorldFromModel);

    // bind color texture to active texture #0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIDs[COLOR_TEXTURE]);

    // bind property texture to active texture #1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureIDs[PROP_TEXTURE]);

    // bind normal texture to active texture #2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureIDs[NORM_TEXTURE]);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferIDs[OBJECT_UNIFORM_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ObjectData), &object);

    Object::draw(app, now);
}

