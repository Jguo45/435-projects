// ray tracer main program
// includes input file parsing and spawning screen pixel rays

// classes used directly by this file
#include "ObjectList.hpp"
#include "Polygon.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "World.hpp"
#include "KDTree.hpp"
#include "Vec3.hpp"

// standard includes
#include <vector>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
// don't complain about MS-deprecated standard C functions
#pragma warning( disable: 4996 )
#endif

int main(int argc, char **argv)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    // parse the intput into everything we know about the world
    // parse command line arguments
    char *filename = nullptr;
    char *progname = argv[0];
    for(++argv, --argc;  argc != 0;  ++argv, --argc) {
        // print usage on -h, -help, -?, --h, --help, etc.
        if (strncmp(argv[0], "-h", 2) == 0 || 
                strncmp(argv[0], "--h", 3) == 0 || 
                strcmp(argv[0], "-?") == 0)
            break;

        if (strcmp(argv[0], "-no-parallel") == 0)
            World::effects &= ~World::PARALLEL;
        else if (strcmp(argv[0], "-no-ambient") == 0)
            World::effects &= ~World::AMBIENT;
        else if (strcmp(argv[0], "-no-diffuse") == 0) 
            World::effects &= ~World::DIFFUSE;
        else if (strcmp(argv[0], "-no-specular") == 0)
            World::effects &= ~World::SPECULAR;
        else if (strcmp(argv[0], "-no-shadow") == 0)
            World::effects &= ~World::SHADOW;
        else if (strcmp(argv[0], "-no-reflect") == 0)
            World::effects &= ~World::REFLECT;
        else if (strcmp(argv[0], "-no-refract") == 0)
            World::effects &= ~World::REFRACT;
        else if (strcmp(argv[0], "-no-polygons") == 0)
            World::effects &= ~World::POLYGONS;
        else if (strcmp(argv[0], "-no-spheres") == 0)
            World::effects &= ~World::SPHERES;
        else if (argc == 1)
            filename = argv[0];
        else
            break;
    }

    // unparsed arguments? print usage and exit
    if (!filename || argc != 0) {
        std::cerr << "Usage: " << progname << " [options] file.ray\n" 
            << "options:\n"
            << "  -no-parallel\n"
            << "  -no-ambient, -no-diffuse, -no-specular\n"
            << "  -no-shadow, -no-reflect, -no-refract\n"
            << "  -no-polygons, -no-spheres\n"
            << "    turn off ray-tracing features\n"
            << "output in trace.ppm\n";
        return 1;
    }

    // input file from command line or stdin
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error opening " << filename << '\n';
        return 1;
    }

    // image parameters, camera parameters
    World world(infile);
    KDTree tree(world.treeObjects);

    // array of image data in ppm-file order
    unsigned char (*pixels)[3] = new unsigned char[world.height*world.width][3];

    // spawn a ray for each pixel and place the result in the pixel
    std::vector<std::thread> threads;
    for (int j=0; j<world.width; ++j) {
        // spawn thread per line
        // capture j by value since it'll change before/during thread execution
        threads.push_back(std::thread([&, j]{
            for(int i=0; i<world.width; ++i) {

                // trace new ray
                float us = world.left + (world.right  - world.left) * (i+0.5f)/world.width;
                float vs = world.top  + (world.bottom - world.top ) * (j+0.5f)/world.height;
                Vec3 dir = -world.dist * world.w + us * world.u + vs * world.v;

                Ray ray(world.eye, dir, 1e-4, INFINITY, world.maxdepth, 1);
                Intersection isect = NULL;
                tree.traverse(isect, tree._root, ray, ray.E + ray.near * ray.D);
                Vec3 col = isect.color(world, ray);

                // assign color
                pixels[j*world.width + i][0] = col.r();
                pixels[j*world.width + i][1] = col.g();
                pixels[j*world.width + i][2] = col.b();
            }

            // some measure of progress on line *completion*
            if (j % 32 == 0) std::cout << "line " << j << '\n';
        }));

        // Not exactly the same as not having any parallel code, but simulate
        // no parallelism by joining each thread immediately
        if (!(World::effects & World::PARALLEL))
            threads.back().join();
    }

    if ((World::effects & World::PARALLEL)) {
        for (auto &thread : threads) thread.join();
    }

    // write ppm file of pixels
    std::ofstream output("trace.ppm", std::ofstream::out | std::ofstream::binary);
    output << "P6\n" << world.width << ' ' << world.height << '\n' << 255 << '\n';
    output.write((const char *)(pixels), world.height*world.width*3);

    delete[] pixels;

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = endTime - startTime;
    std::cout << elapsed.count() << " seconds\n";
    return 0;
}

