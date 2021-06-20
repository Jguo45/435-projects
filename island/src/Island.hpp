// data and functions to draw an island
#pragma once

#include "Object.hpp"
#include <map>

// island object
class Island : public Object {
public:
	// create island from -size/2 to size/2
	Island(glm::vec3 size, const char* texturePPM);

	// returns all triangles containing vertex at the given index
	std::vector<unsigned int> containsVertex(int index);

	// computes the area weighted normal of the vertex at the given index
	glm::vec3 vectorNormal(int index);

	// creates vertices at the midpoints of each edge
	void increaseSubdivisions();
	// reverts island back to previous level
	void decreaseSubdivisions(VectorData cached);
	// sends the new uv and index array data to gpu
	void updateIsland();

	// returns the midpoint of 2 2d vectors
	glm::vec2 vec2Midpoint(glm::vec2 v0, glm::vec2 v1) {
		return glm::vec2((v0.x + v1.x) / 2, (v0.y + v1.y) / 2);
	};
	// returns the midpoint of 2 3d vectors
	glm::vec3 vec3Midpoint(glm::vec3 v0, glm::vec3 v1) {
		return glm::vec3((v0.x + v1.x) / 2, (v0.y + v1.y) / 2, (v0.z + v1.z) / 2);
	};

public:
	// comparison operator to be used by map for vec2
	struct vec2Compare {
		bool operator() (const glm::vec2& v0, const glm::vec2& v1) const {
			return sqrt(v0.x * v0.x + v0.y * v0.y) < sqrt(v1.x * v1.x + v1.y * v1.y);
		}
	};

	// map with uv coordinates as keys and indexes as values
	std::map<glm::vec2, int, vec2Compare> existingVertices;
};