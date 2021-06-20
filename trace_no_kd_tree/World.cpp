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
#include <string>
#include <map>

// read input file
World::World(std::istream &ifile)
: eye(0,-8,0), width(512), height(512)
{
    // temporary variables while parsing
    Vec3 look(0,0,0), up(0,1,0);
    float xfov=45, yfov=45;
    std::string surfname;

    // map of surface names to colors, only need while parsing
    std::map<std::string, Surface> surfaceMap;
    //Vec3 *currentColor = &surfaceMap[""];

    std::string token;
    while(ifile >> token) {
        if (token == "background")
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
        else if (token == "maxdepth")
            ifile >> maxDepth;
        else if (token == "cutoff")
            ifile >> cutOff;
        else if (token == "light") {
            float intensity;
            Vec3 position;
            std::string type;

            ifile >> intensity >> type >> position;

            lights.push_back(Light(intensity, position));
        }

        else if (token == "surface") {
            ifile >> surfname;
            surfaceMap[surfname] = Surface(surfname);
        }

        else if (token == "ambient")
            ifile >> surfaceMap[surfname]._ambient;
        else if (token == "diffuse")
            ifile >> surfaceMap[surfname]._diffuse;
        else if (token == "specular")
            ifile >> surfaceMap[surfname]._specular;
        else if (token == "specpow")
            ifile >> surfaceMap[surfname]._specpow;
        else if (token == "reflect")
            ifile >> surfaceMap[surfname]._reflect;

        //else if (token == "polygon") {
        //    ifile >> surfname;
        //    Polygon *poly = new Polygon(surfaceMap[surfname]);
        //    Vec3 vert;
        //    while (ifile >> vert)
        //        poly->addVertex(vert);
        //    ifile.clear();
        //    poly->closePolygon();
        //    objects.addObject(poly);
        //}

        else if (token == "sphere") {
            float radius;
            Vec3 center;
            ifile >> surfname >> radius >> center;
            objects.addObject(new Sphere(surfaceMap[surfname], center, radius));
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
}


