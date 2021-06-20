// build and draw a fractal island model

#include "Island.hpp"
#include "GLapp.hpp"
#include "config.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

#include <iostream>

using namespace glm;  // avoid glm:: for all glm types and functions

// custom hash for texture coordinate rounded to a grid
size_t Island::hash::operator()(vec2 v) const
{
    // round to grid
    ivec2 iv = ivec2(1048576.f * v + 0.5f);
    return std::hash<ivec2>()(iv);
}

// custom equality comparison for texture coordinates rounded to a grid
bool Island::equal::operator()(vec2 v0, vec2 v1) const
{
    ivec2 iv0 = ivec2(1048576.f * v0 + 0.5f);
    ivec2 iv1 = ivec2(1048576.f * v1 + 0.5f);
    return iv0 == iv1;
}

// create hard-coded base island
Island::Island(vec3 size, const char *texturePPM, const char* propPPM, const char* normPPM) :
    Object(texturePPM, propPPM, normPPM)
{
    zscale = size.z;
    
    // build texture coordinate, normal, tangent, bitangent, and vertex arrays
    // base edge, texture coordinate, vertex, and index arrays
    edge = {false, true, true, true, true, true, true};
    uv = {
        vec2(0.5f, 0.5f), 
        vec2(0.25f, 0.5f+0.25f*sqrtf(3.f)),
        vec2(0.75f, 0.5f+0.25f*sqrtf(3.f)), 
        vec2(1.0f, 0.5f),
        vec2(0.75f, 0.5f-0.25f*sqrtf(3.f)),
        vec2(0.25f, 0.5f-0.25f*sqrtf(3.f)),
        vec2(0.0f, 0.5f)
    };
    vert = {
        size * vec3(uv[0].x - 0.5f, 0.5f - uv[0].y, 1.f),
        size * vec3(uv[1].x - 0.5f, 0.5f - uv[1].y, 0.f),
        size * vec3(uv[2].x - 0.5f, 0.5f - uv[2].y, 0.f),
        size * vec3(uv[3].x - 0.5f, 0.5f - uv[3].y, 0.f),
        size * vec3(uv[4].x - 0.5f, 0.5f - uv[4].y, 0.f),
        size * vec3(uv[5].x - 0.5f, 0.5f - uv[5].y, 0.f),
        size * vec3(uv[6].x - 0.5f, 0.5f - uv[6].y, 0.f)
    };
    indices = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 1
    };

    // build base island
    baseVertices = uint(vert.size());
    indexStart = {0u, uint(indices.size())};
    norm.resize(baseVertices);
    tan.resize(baseVertices);
    bitan.resize(baseVertices);
    setLevel(0u);
    updateShaders();
}

// create island from obj file
Island::Island(const char *islandOBJ, const char *texturePPM) :
    Object(texturePPM)
{
    // open obj in project data directory
    std::string path = std::string(PROJECT_DATA_DIR) + islandOBJ;
    FILE *fp = fopen(path.c_str(), "r");

    // if data path didn't work, try direct path
    if (!fp) fp = fopen(islandOBJ, "r");
    assert(fp);

    // read lines from file, ignoring everything but 'v', 'vt', or 'f' lines
    std::vector<vec3> objv;
    std::vector<vec2> objvt;
    std::unordered_map<std::string, int> facemap;
    std::unordered_map<ivec2, int> edgemap;
    char line[1024], vvt[3][1024];
    int idx[3];
    while(fgets(line, sizeof(line), fp)) {
        float x, y, z;
        // try parsing as vertex line
        if (sscanf(line, "v %f %f %f", &x, &y, &z) == 3) {
            objv.push_back(vec3(x,y,z));
            zscale = std::max(zscale, z);
        }

        // try parsing as texture coordinate line
        else if (sscanf(line, "vt %f %f", &x, &y) == 2)
            objvt.push_back(vec2(x,y));

        // try parsing as face line
        else if (sscanf(line, "f %s %s %s", vvt[0], vvt[1], vvt[2]) == 3) {
            for(int i=0; i<3; ++i) {
                // create a new vertex if we haven't seen it before
                if (! facemap.count(vvt[i])) {
                    // add mapping from v/vt to real index
                    facemap[vvt[i]] = vert.size();

                    // get vertex and texture coordinate indices
                    int v=1, vt=1;
                    sscanf(vvt[i], "%d/%d", &v, &vt);
                    vert.push_back(objv[v-1]);
                    uv.push_back(objvt[vt-1]);
                    edge.push_back(false);
                }

                // add to the index list
                idx[i] = facemap[vvt[i]];
                indices.push_back(idx[i]);
            }
            for(int j=2, i=0; i<3; j=i, ++i) {
                // count how many times each edge appears
                ++edgemap[ivec2(std::min(idx[i], idx[j]), std::max(idx[i], idx[j]))];
            }
        }
    }
    fclose(fp);
    
    // external edges will have edgemap count of 1
    for(auto e : edgemap) {
        if (e.second == 1) {
            edge[e.first.x] = true;
            edge[e.first.y] = true;
        }
    }
    
    // build base island
    baseVertices = uint(vert.size());
    indexStart = {0u, uint(indices.size())};
    norm.resize(baseVertices);
    setLevel(0u);
    updateShaders();
}

// select level to render
void Island::setLevel(unsigned int _level)
{
    // generate missing levels
    for(unsigned int i = indexStart.size(); i < _level+2; ++i)
        addLevel();
    level = _level;

    // zero out existing normals, tangents, and bitangents
    for(auto &N : norm)
        N = vec3(0);
    for (auto& T : tan)
        T = vec3(0);
    for (auto& B : bitan)
        B = vec3(0);

    // accumulate polygon normal, tangent, and bitangent to each affected vertex
    for(int i=indexStart[level]; i < indexStart[level+1]; i += 3) {
        vec3 v0 = vert[indices[i+0]];
        vec3 v1 = vert[indices[i+1]];
        vec3 v2 = vert[indices[i+2]];

        vec3 N = cross(v1-v0, v2-v0);
        vec3 T = cross(vec3(1.f, 0.f, 0.f), N);
        vec3 B = cross(vec3(0.f, 1.f, 0.f), N);

        norm[indices[i + 0]] += N;
        tan[indices[i + 0]] += T;
        bitan[indices[i + 0]] += B;

        norm[indices[i + 1]] += N;
        tan[indices[i + 1]] += T;
        bitan[indices[i + 1]] += B;

        norm[indices[i + 2]] += N;
        tan[indices[i + 2]] += T;
        bitan[indices[i + 2]] += B;
    }

    // renormalize normals, tangents, and bitangents
    for(auto &N : norm)
        N = normalize(N);
    for (auto& T : tan)
        T = normalize(T);
    for (auto& B : bitan)
        B = normalize(B);

    // load arrays to GPU
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indexStart[level+1] - indexStart[level]) * sizeof(indices[0]), &indices[indexStart[level]], GL_STATIC_DRAW);
}

// reset random seed
void Island::reseed(unsigned int seed)
{
    // reseed generator
    randgen.seed(seed);
    
    // trim array sizes back to base
    edge.resize(baseVertices);
    uv.resize(baseVertices);
    vert.resize(baseVertices);
    norm.resize(baseVertices);
    tan.resize(baseVertices);
    bitan.resize(baseVertices);
    indices.resize(indexStart[1]);
    indexStart.resize(2);
    vertexMap.clear();
    
    // re-generate to current level
    setLevel(level);
}

// add one new level
void Island::addLevel()
{
    int prevStart = indexStart[indexStart.size()-2];
    int prevEnd = indexStart.back();
    
    auto rand = std::bind(std::uniform_real_distribution<float>(-1.f, 1.f), randgen);

    // for each old triangle, insert four new ones
    for(int i=prevStart; i<prevEnd; i+=3) {
        // find or generate new vertices
        unsigned int mid[3];    // indices for midpoints on 2-0, 0-1, and 1-2 edges
        unsigned int i0 = indices[i+2], i1 = indices[i];
        for(int j=0; j<3; ++j, i0=i1, i1=indices[i+j]) {
            vec2 miduv = 0.5f * (uv[i0] + uv[i1]);
            float levelscale = length(uv[i0] - uv[i1]);

            // need to generate a new midpoint vertex?
            if (! vertexMap.count(miduv)) {
                vertexMap[miduv] = uv.size();
                uv.push_back(miduv);
                norm.push_back(vec3());
                tan.push_back(vec3());
                bitan.push_back(vec3());

                bool isedge = edge[i0] && edge[i1];
                edge.push_back(isedge);
                
                vec3 midvert = 0.5f * (vert[i0] + vert[i1]);
                //float rand = float((unsigned int)(hash()(miduv)) & 0xffffu) / 0xffff - 0.5f;
                midvert.z += zscale * levelscale * rand();
                if (isedge && midvert.z > 0.f) midvert.z = 0.f;
                vert.push_back(midvert);
            }
            mid[j] = vertexMap[miduv];
        }

        // insert new triangles
        indices.push_back(indices[i+0]); indices.push_back(mid[1]); indices.push_back(mid[0]);
        indices.push_back(indices[i+1]); indices.push_back(mid[2]); indices.push_back(mid[1]);
        indices.push_back(indices[i+2]); indices.push_back(mid[0]); indices.push_back(mid[2]);
        indices.push_back(mid[0]); indices.push_back(mid[1]); indices.push_back(mid[2]);
    }
    indexStart.push_back(indices.size());
}

float Island::getHeight(float x, float y)
{
    // loops through all the indices at the current level
    for (int i = indexStart[level]; i < indexStart[level + 1]; i += 3) {
        // vertices of the current triangle
        vec3 v0 = vert[indices[i + 0]];
        vec3 v1 = vert[indices[i + 1]];
        vec3 v2 = vert[indices[i + 2]];

        // area of the current triangle
        float triArea = (v0.x * v1.y - v0.y * v1.x) +
                        (v1.x * v2.y - v1.y * v2.x) +
                        (v2.x * v0.y - v2.y * v0.x);
        // barycentric coordinates: alpha, beta, gamma
        float a, b, c;

        // calculate alpha
        a = (x * v1.y - y * v1.x) + 
            (v1.x * v2.y - v1.y * v2.x) +
            (v2.x * y - v2.y * x);
        a = a / triArea;

        if (a < 0 || a > 1) continue;   // current position is outside of the triangle

        // calculate beta
        b = (x * v2.y - y * v2.x) +
            (v2.x * v0.y - v2.y * v0.x) +
            (v0.x * y - v0.y * x);
        b = b / triArea;

        if (b < 0 || b > 1) continue;   // current position is outside of the triangle
        
        // calculate gamma
        c = 1 - b - a;                  // weights should add up to 1

        if (c < 0 || c > 1) continue;   // current position is outside of the triangle

        vec3 height = a * v0 + b * v1 + c * v2; // determine the height of the triangle at the current position

        // clamps height to 0 if height is below sea level
        if (height.z < 0)
            height.z = 0;

        return height.z;
    }
    // current position is off of the island entirely
    return 0.f;
}

void Island::updateShaders()
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

// replicate object draw to use just a subset of the indices
void Island::draw(GLapp* app, double now)
{
    // enable shader
    glUseProgram(shaderID);

    // select vertex array to render
    glBindVertexArray(varrayID);

    // bind color texture to active texture #0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIDs[COLOR_TEXTURE]);

    // bind property texture to active texture #1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureIDs[PROP_TEXTURE]);

    // bind normal texture to active texture #2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureIDs[NORM_TEXTURE]);

    // bind uniform buffers to the appropriate uniform block numbers
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, app->sceneUniformsID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferIDs[OBJECT_UNIFORM_BUFFER]);

    // draw the triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, indexStart[level+1] - indexStart[level], GL_UNSIGNED_INT, 0);
}
