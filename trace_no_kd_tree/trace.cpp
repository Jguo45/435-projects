// ray tracer main program
// includes input file parsing and spawning screen pixel rays

// classes used directly by this file
#include "ObjectList.hpp"
#include "Polygon.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "World.hpp"
#include "Vec3.hpp"

// standard includes
#include <fstream>
#include <iostream>
#include <chrono>

// checks if there is an object between start and end
bool probe(Vec3 start, Vec3 end, float distance, World& world) {
	Ray ray(start, end);
	return world.objects.probe(ray, distance);
}

// finds the correct color given the start position and direction vector
Vec3 trace(Vec3 origin, Vec3 dir, World& world, int bounceCount, float contrib) {
	Ray ray(origin, dir);
	Intersection isect = world.objects.trace(ray);

	// hits no objects
	if (isect.obj == nullptr) {
		return world.background;
	}

	// ends recursion
	else if (bounceCount > world.maxDepth || contrib < world.cutOff) {
		return Vec3(1, 1, 1);
	}

	// hits an object
	else {
		Vec3 col = isect.obj->color._ambient;				// base color of surface
		Vec3 P = origin + (isect.t * dir);					// point of intersection
		Vec3 N = normalize(P - isect.obj->getCenter());		// unit normal vector of object at P
		Vec3 V = normalize(origin - dir);					// unit view vector
		Vec3 rDir = -V + 2 * (dot(N, V) * N);				// direction of reflection

		// calculates lighting
		for (Light light : world.lights) {
			Vec3 L = normalize(light._pos - P);				// unit vector from light source

			// checks if point P is in shadow
			if (probe(P, light._pos, length(light._pos - P), world)) {
				if (dot(N, L) > 0) {
					col = col + (light._intensity * isect.obj->color._diffuse * dot(N, L));
					Vec3 H = normalize(L + V);				// unit half vector
					if (dot(N, H) > 0) {
						col = col + (light._intensity * isect.obj->color._specular * (pow(dot(N, H), isect.obj->color._specpow)));
					}
				}
			}
		}
		// checks if surface is reflective
		if (isect.obj->color._reflect > 0) {
			contrib *= isect.obj->color._reflect;
			bounceCount++;
			col = col + (isect.obj->color._reflect * trace(P, rDir, world, bounceCount, contrib));
		}

		return col;
	}
}

int main(int argc, char** argv)
{
	auto startTime = std::chrono::high_resolution_clock::now();

	if (argc <= 1) {
		std::cerr << "Usage: trace <file.ray>\n";
		std::cerr << "    Where <file.ray> is a rayshade scene description file\n";
		return 1;
	}

	// input file from command line or stdin
	std::ifstream infile(argv[1]);
	if (!infile) {
		std::cerr << "Error opening " << argv[1] << '\n';
		return 1;
	}

	// parse the input into everything we know about the world
	// image parameters, camera parameters
	World world(infile);

	// array of image data in ppm-file order
	unsigned char(*pixels)[3] = new unsigned char[world.height * world.width][3];

	// spawn a ray for each pixel and place the result in the pixel
	for (int j = 0; j < world.height; ++j) {
		for (int i = 0; i < world.width; ++i) {

			// trace new ray
			float us = world.left + (world.right - world.left) * (i + 0.5f) / world.width;
			float vs = world.top + (world.bottom - world.top) * (j + 0.5f) / world.height;
			Vec3 dir = -world.dist * world.w + us * world.u + vs * world.v;

			// determines color at pixel location
			Vec3 col = trace(world.eye, dir, world, 0, 1);

			// assign color
			pixels[j * world.width + i][0] = col.r();
			pixels[j * world.width + i][1] = col.g();
			pixels[j * world.width + i][2] = col.b();
		}

		if (j % 32 == 0) std::cout << "line " << j << '\n'; // render progress
	}

	// write ppm file of pixels
	std::ofstream output("trace.ppm", std::ofstream::out | std::ofstream::binary);
	output << "P6\n" << world.width << ' ' << world.height << '\n' << 255 << '\n';
	output.write((const char*)(pixels), world.height * world.width * 3);

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = endTime - startTime;
	std::cout << elapsed.count() << " seconds\n";

	delete[] pixels;
	return 0;
}

