// collected state for access in callbacks
// 
#pragma once

#include <glm/glm.hpp>
#include <vector>

class GLapp {
public:
    struct GLFWwindow *win;      // graphics window from GLFW system

    // uniform buffer data about the scene
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct SceneData {
        glm::mat4 ProjFromWorld, WorldFromProj;  // viewing matrix & inverse
        glm::vec4 LightDir;                      // xyz = light direction; w = ambient
        float Col;          // flag for color
        float AO;           // flag for ambient occlusion
        float Gloss;        // flag for gloss
        float Norm;         // flag for normal
    } scene;
    unsigned int sceneUniformsID;

    // view info
    int width, height;         // current window dimensions
    float distance;            // distance from 0,0,0
    float pan, tilt;           // horizontal and vertical Euler angles
    float panRate, tiltRate;    // keyboard orbiting rate in radians/sec

    float xMoveRate, yMoveRate; // x and y move rate of the sphere
    bool strafe;                // if 'A' or 'D' is pressed

    // mouse state
    bool button;                // is the button currently pressed?
    double mouseX, mouseY;      // location of mouse at last event

    // move state
    bool moving;

    // drawing state
    bool wireframe;
    unsigned int level, seed;

    // time (in seconds) of last frame
    double prevTime;

    // objects to draw
    class Island *island;
    class Sphere *sphere;
    std::vector<class Object*> objects;

public:
    // initialize and destroy app data
    GLapp();
    ~GLapp();

    // update shader uniform state each frame
    void sceneUpdate(double dTime);

    // main rendering loop
    void render();
};
