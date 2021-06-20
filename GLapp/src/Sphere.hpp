// data and functions to draw a sphere
#pragma once

#include "Object.hpp"

// sphere object
class Sphere : public Object {
public:
    // create sphere given latitude and longitude sizes and color texture, property texture, and normal texture
    Sphere(int width, int height, glm::vec3 size, const char *texturePPM, const char *propPPM, const char* normPPM);

    // updates shader for this sphere object
    virtual void updateShaders() override;

    // draw this sphere object
    virtual void draw(GLapp *app, double now) override;

    // current position of the sphere
    glm::vec3 position;
};
