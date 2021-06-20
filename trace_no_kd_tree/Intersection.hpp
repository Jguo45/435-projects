// intersection results
#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

// other classes we use DIRECTLY in our interface
#include "Vec3.hpp"

// classes we only use by pointer or reference
class World;
class Object;
class Ray;

// intersection results: contains object hit and t of first intersection point
class Intersection {
public: // public data
    float t;                // where along ray?

//private: // private data
    const Object *obj;    // what did we hit?

public: // constructors
    // default construct with no object, intersection at infinity
    Intersection(const Object *_obj=0, float _t=INFINITY);

    // we also also allow default copy constructor and assignment

public: // computational members
    // get color for this intersection
    const Vec3 color(const World&) const;
};

// compare two intersections by comparing t distance
inline bool operator<(const Intersection i0, const Intersection i1) {
    return i0.t < i1.t;
}

#endif
