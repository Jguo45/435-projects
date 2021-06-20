// Rays
#ifndef RAY_HPP
#define RAY_HPP

// other classes we use DIRECTLY in our interface
#include "Vec3.hpp"

// a ray consists of a starting point and direction
// ray is defined as start + t*direction, for t>=0
class Ray {
public: // public data
    Vec3 E;		// ray start point
    Vec3 D;     // ray direction

    // derived, for intersection testing
    float D_dot_D;

public: // constructors
    Ray(const Vec3 &_start, const Vec3 &_direction) {
        E = _start;
        D = _direction;
        D_dot_D = dot(D,D);
    }
};


#endif
