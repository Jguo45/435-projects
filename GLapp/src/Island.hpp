// data and functions to draw a fractal island
#pragma once

#include "Object.hpp"
#include <random>
#include <unordered_map>

// island object
class Island : public Object {
    // scale for position changes
    float zscale;
    
	// flag for edge vertices
	std::vector<bool> edge;

    // number of vertices in base level
    glm::uint baseVertices;

	// start in index array of each level
	std::vector<glm::uint> indexStart;
	unsigned int level;

	// map from texture coordinate to vertex index
	struct hash {
		size_t operator()(glm::vec2) const;
	};
	struct equal {
		bool operator()(glm::vec2, glm::vec2) const;
	};
	std::unordered_map<glm::vec2, glm::uint, hash, equal> vertexMap;

    // random number generator
    std::default_random_engine randgen;
    
public:
    // create island from -size/2 to size/2 with property and normal file
    Island(glm::vec3 size, const char *texturePPM, const char* propPPM, const char* normPPM);

    // create island from obj file
    Island(const char *islandOBJ, const char *texturePPM);

    // reset random seed
    void reseed(unsigned int seed);
    
    // select or generate island level
    void setLevel(unsigned int level);

    // generate next new level
    void addLevel();

    // gets the height of the island at the given (x, y) coordinate
    float getHeight(float x, float y);

    // updates shaders for this island object
    virtual void updateShaders() override;

    // draw current island
    virtual void draw(class GLapp *app, double now) override;
};
