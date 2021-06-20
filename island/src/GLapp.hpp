// collected state for access in callbacks
// 
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Island.hpp"


class GLapp {
public:
    struct GLFWwindow *win;      // graphics window from GLFW system

    // uniform buffer data about the scene
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct SceneData {
        glm::mat4 ProjFromWorld, WorldFromProj;  // viewing matrix & inverse
        glm::vec4 LightDir;         // xyz = light direction; w = ambient
    } scene;
    unsigned int sceneUniformsID;

    // view info
    int width, height;         // current window dimensions
    float distance;            // distance from 0,0,0
    float pan, tilt;           // horizontal and vertical Euler angles
    float panRate, tiltRate;    // keyboard orbiting rate in radians/sec

    const char* textureFile;

    // mouse state
    bool button;                // is the button currently pressed?
    double mouseX, mouseY;      // location of mouse at last event

    // drawing state
    bool wireframe;

    // time (in seconds) of last frame
    double prevTime;

    // objects to draw
    std::vector<class Object*> objects;

    // stores previously generated islands
    std::vector<class VectorData> islandCache;

    // current level
    int level;
    
    // Island
    Island* island;

public:
    // initialize and destroy app data
    GLapp();
    ~GLapp();

    // update shader uniform state each frame
    void sceneUpdate(double dTime);

    // main rendering loop
    void render();
};

