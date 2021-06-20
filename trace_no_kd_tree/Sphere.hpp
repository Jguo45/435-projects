// sphere objects
#ifndef SPHERE_HPP
#define SPHERE_HPP

// other classes we use DIRECTLY in the interface
#include "Object.hpp"
#include "Vec3.hpp"

const float BIAS = 0.00001f;

// classes we only use by pointer or reference
class World;
class Ray;

// sphere objects
class Sphere : public Object {
public:
    Vec3 C;
    float R;

    // derived, for intersection testing
    float Rsquared;

public: // constructors
    Sphere(const Surface &_surface, const Vec3 &_center, float _radius);

public: // object functions
    const Intersection intersect(const Ray &ray) const;
    const Vec3 getCenter() const;
    bool probe(const Ray& ray, float distance);
};

#endif
