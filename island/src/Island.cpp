
#include "Island.hpp"
#include "GLapp.hpp"
#include <math.h>
#include <iostream>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace glm;

// load island data
Island::Island(glm::vec3 size, const char* texturePPM) : Object(texturePPM) {
	level = 0;

	// uv coordinates for level 0 island 
	uv = {
		vec2(0.5f, 0.5f),
		vec2(0.f, 0.5f),
		vec2(0.5f - 0.25f, 0.5f - 0.25 * sqrt(3)),
		vec2(0.5f + 0.25f, 0.5f - 0.25 * sqrt(3)),
		vec2(1.f, 0.5f),
		vec2(0.5f + 0.25f, 0.5f + 0.25 * sqrt(3)),
		vec2(0.5f - 0.25f, 0.5f + 0.25 * sqrt(3))
	};

	// vertex coordinates for level 0 island
	vert = {
		vec3(0.f, 0.f, size.z),
		vec3(-0.5f * size.x, 0.f, 0.f),
		vec3(-0.25f * size.x, 0.5f * (sqrt(3) / 2) * size.y, 0.f),
		vec3(0.25f * size.x, 0.5f * (sqrt(3) / 2) * size.y, 0.f),
		vec3(0.5f * size.x, 0.f, 0.f),
		vec3(0.25f * size.x, -0.5f * (sqrt(3) / 2) * size.y, 0.f),
		vec3(-0.25f * size.x, -0.5f * (sqrt(3) / 2) * size.y, 0.f)
	};

	// indices of triangles that form level 0 island in counter-clockwise order
	indices = {
		0, 2, 1,
		0, 3, 2,
		0, 4, 3, 
		0, 5, 4, 
		0, 6, 5,
		0, 1, 6
	};

	// compute normals for each vector
	for (int i = 0; i < vert.size(); ++i) {
		vec3 N = vectorNormal(i);
		norm.push_back(N);
	}

	// adds uv coordinates to map
	for (int i = 0; i < uv.size(); ++i) {
		existingVertices[uv[i]] = i;
	}

	updateIsland();
}

std::vector<unsigned int> Island::containsVertex(int index) {
	std::vector<unsigned int> faces;					// vector containing indices of all faces with given vertex

	// loops through every face on the island checking if it contains the given vertex
	for (int i = 0; i < indices.size() / 3; ++i) {
		if (indices[i * 3 + 0] == index ||
			indices[i * 3 + 1] == index ||
			indices[i * 3 + 2] == index) 
		{
			faces.push_back(indices[i * 3 + 0]);
			faces.push_back(indices[i * 3 + 1]);
			faces.push_back(indices[i * 3 + 2]);
		}
	}
	return faces;
}

vec3 Island::vectorNormal(int index) {
	vec3 sum(0);
	std::vector<unsigned int> faces = containsVertex(index);

	// loops through every face containing the given vertex and totals its cross product
	for (int i = 0; i < faces.size() / 3; ++i) {

		vec3 v0 = vert[faces[i * 3 + 0]];
		vec3 v1 = vert[faces[i * 3 + 1]];
		vec3 v2 = vert[faces[i * 3 + 2]];

		vec3 N = cross(v1 - v0, v2 - v0);

		sum += N;

	}

	return normalize(sum);
}

void Island::increaseSubdivisions() {
	std::vector<unsigned int> newIndices;			

	// loops through each face, adding a vertex at the midpoint of each edge
	for (int i = 0; i < indices.size() / 3; ++i) {
		std::vector<vec2> uvMid;					// vector containing all midpoints for uv array
		std::vector<vec3> vMid;						// vector containing all midpoints for vertex array
		std::vector<unsigned int> tmpIndices;		// vector containing updated vertex

		// indices of the vertices at the current face
		unsigned int i0 = indices[i * 3 + 0];
		unsigned int i1 = indices[i * 3 + 1];
		unsigned int i2 = indices[i * 3 + 2];

		// vec3 vertices
		vec3 v0 = vert[i0];
		vec3 v1 = vert[i1];
		vec3 v2 = vert[i2];

		// vec2 vertices
		vec2 uv0 = uv[i0];
		vec2 uv1 = uv[i1];
		vec2 uv2 = uv[i2];

		// compute midpoints for each side
		vMid.push_back(vec3Midpoint(v0, v1));
		vMid.push_back(vec3Midpoint(v1, v2));
		vMid.push_back(vec3Midpoint(v2, v0));

		uvMid.push_back(vec2Midpoint(uv0, uv1));
		uvMid.push_back(vec2Midpoint(uv1, uv2));
		uvMid.push_back(vec2Midpoint(uv2, uv0));

		// for each midpoint computed, check if it already exists
		// displaces z value of vertex if it doesn't exist
		// uses existing z value if it does
		for (int j = 0; j < uvMid.size(); ++j) {
			if (existingVertices.find(uvMid[j]) == existingVertices.end()) {
				float displace = (rand() % 101) * pow(2, -1 * level);		// random amount of vertical displacement

				// randomly determines if it is positive or negative displacement
				if (rand() % 2)
					displace = displace * -1;

				vMid[j].z += displace;

				uv.push_back(uvMid[j]);
				vert.push_back(vMid[j]);
				tmpIndices.push_back(uv.size() - 1);
				existingVertices[uvMid[j]] = uv.size() - 1;
			}
			else {
				tmpIndices.push_back(existingVertices[uvMid[j]]);
			}
		}

		// forms the center triangle
		for (int j = 0; j < tmpIndices.size(); ++j) {
			newIndices.push_back(tmpIndices[j]);
		}

		// forms the surrounding triangles
		newIndices.push_back(tmpIndices[0]);
		newIndices.push_back(tmpIndices[2]);
		newIndices.push_back(i0);

		newIndices.push_back(tmpIndices[1]);
		newIndices.push_back(tmpIndices[0]);
		newIndices.push_back(i1);

		newIndices.push_back(tmpIndices[2]);
		newIndices.push_back(tmpIndices[1]);
		newIndices.push_back(i2);
	}

	// updates the list of indices with the new triangles
	indices = newIndices;

	norm.clear();
	// re-calculates the normals with new vertices
	for (int i = 0; i < vert.size(); ++i) {
		vec3 N = vectorNormal(i);
		norm.push_back(N);
	}

	// clamps perimeter vertices to height 0 if displaced above sea level
	for (int i = 0; i < vert.size(); ++i) {
		if (containsVertex(i).size() / 3 <= 3) {
			if (vert[i].z >= 0) {
				vert[i].z = 0;
			}
		}
	}
}

// takes in VectorData of previous subdivision stored in cache and sets array data
void Island::decreaseSubdivisions(VectorData data) {
	vert = data.vert;
	norm = data.norm;
	uv = data.uv;
	indices = data.indices;

	updateIsland();
}

void Island::updateIsland() {
	glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), &vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(norm[0]), &norm[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(uv[0]), &uv[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	updateShaders();
}