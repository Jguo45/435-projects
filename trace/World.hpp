// Everything we know about the world
#ifndef World_hpp
#define World_hpp

// other classes we use DIRECTLY in our interface
#include "Vec3.hpp"
#include "ObjectList.hpp"
#include "KDTree.hpp"
#include <fstream>
#include <vector>

struct Light {
    Vec3 col;                   // light color
    Vec3 pos;                   // light position
    Light() : col(1,1,1), pos(0,0,0) {}
    Light(Vec3 _col, Vec3 _pos) : col(_col), pos(_pos) {}
};
typedef std::vector<Light> LightList;

class World {
public: // public data
    enum Effects {                          // one bit for each feature
        PARALLEL       = 0x001,
        AMBIENT        = 0x002, 
        DIFFUSE        = 0x004, 
        SPECULAR       = 0x008, 
        SHADOW         = 0x010, 
        REFLECT        = 0x020, 
        REFRACT        = 0x040,
        POLYGONS       = 0x080,
        SPHERES        = 0x100
    };
    static unsigned int effects;

    // image size
    int width, height;

    // background color
    Vec3 background;

    // view origin and basis parameters
    Vec3 eye, w, u, v;
    float dist, left, right, bottom, top;

    // ray recursion termination
    int maxdepth;
    float cutoff;


    // list of objects in the scene
    ObjectList *objects;
    ObjectList* treeObjects;

    // list of lights
    LightList lights;

public:                                                     
    // read world data from a file
    World(std::istream &ifile); 
};

#endif
