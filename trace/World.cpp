// implementation code World object
// World holds everything we know about the world.
// Code here initializes it based on file input

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "World.hpp"

// local includes
#include "Polygon.hpp"
#include "Sphere.hpp"

// system includes
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

// scoped global for what is enabled
unsigned int World::effects = ~0;

// read input file
World::World(std::istream &ifile)
{
    int SphereCount = 0, PolyCount = 0;
    objects = new ObjectList();
    treeObjects = new ObjectList();

    // world state defaults
    eye = Vec3(0,-8,0);
    width = height = 512;
    maxdepth = 15;
    cutoff = 0.002;

    // temporary variables while parsing
    Vec3 look(0,0,0), up(0,1,0);
    float xfov=45, yfov=45;
    std::string surfname;

    // map of surface names to colors, only need while parsing
    std::map<std::string, Surface> surfaceMap;
    Surface *currentSurface = &surfaceMap[""];

    std::string token;
    while(ifile >> token) {
        if (token == "maxdepth")
            ifile >> maxdepth;
        else if (token == "cutoff")
            ifile >> cutoff;

        else if (token == "background")
            ifile >> background;
        else if (token == "eyep")
            ifile >> eye;
        else if (token == "lookp")
            ifile >> look;
        else if (token == "up")
            ifile >> up;
        else if (token == "fov")
            ifile >> xfov >> yfov;
        else if (token == "screen")
            ifile >> width >> height;

        else if (token == "surface") {
            ifile >> surfname;
            currentSurface = &surfaceMap[surfname];
        }
            
        else if (token == "ambient")
            ifile >> currentSurface->ambient;
        else if (token == "diffuse")
            ifile >> currentSurface->diffuse;
        else if (token == "specular")
            ifile >> currentSurface->specular;
        else if (token == "specpow")
            ifile >> currentSurface->e;
        else if (token == "reflect")
            ifile >> currentSurface->kr;
        else if (token == "transp")
            ifile >> currentSurface->kt;
        else if (token == "index")
            ifile >> currentSurface->ir;

        else if (token == "light") {
            float intensity;
            Vec3 position;
            ifile >> intensity >> token >> position;
            lights.push_back(Light(Vec3(intensity, intensity, intensity), position));
        }
        
        //else if (token == "polygon") {
        //    ifile >> surfname;
        //    Polygon *poly = new Polygon(surfaceMap[surfname]);
        //    Vec3 vert;
        //    while (ifile >> vert)
        //        poly->addVertex(vert);
        //    ifile.clear();
        //    poly->closePolygon();
        //    if ((World::effects & World::POLYGONS)) {
        //        ++PolyCount;
        //        objects.addObject(poly);
        //    }
        //}

        else if (token == "sphere") {
            float radius;
            Vec3 center;
            ifile >> surfname >> radius >> center;
            if ((World::effects & World::SPHERES)) {
                ++SphereCount;
                objects->addObject(new Sphere(surfaceMap[surfname], center, radius));
                treeObjects->addObject(new Sphere(surfaceMap[surfname], center, radius));
            }
        }
    }

    // compute view basis
    w = eye - look;
    dist = length(w);
    w = normalize(w);
    u = normalize(cross(up, w));
    v = cross(w, u);

    // solve for screen edges
    right = dist * tanf(xfov * M_PI/360);
    left = -right;
    top = dist * tanf(yfov * M_PI/360);
    bottom = -top;

    std::cout << objects->objects.size() << " Objects (" 
        << SphereCount << " Sphere" << (SphereCount == 1 ? "" : "s") << ", " 
        << PolyCount << " Polygon" << (PolyCount == 1 ? "" : "s") << "); "
        << lights.size() << " Light" << (lights.size() == 1 ? "" : "s") << '\n';
}
