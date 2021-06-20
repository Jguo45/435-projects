// sphere objects
#ifndef SPHERE_HPP
#define SPHERE_HPP

// other classes we use DIRECTLY in the interface
#include "Object.hpp"
#include "Vec3.hpp"

// classes we only use by pointer or reference
class Appearance;
class World;
class Ray;

// sphere objects
class Sphere : public Object {
    Vec3 C;
    float R;

    // derived, for intersection testing
    float Rsquared;

public: // constructors
    Sphere(const Surface &_surface, const Vec3 _center, float _radius);

public: // object functions
    const Intersection intersect(const Ray &ray) const override;
    const Vec3 normal(const Vec3 P) const override;
    Vec3 getCenter() override;
    float getRadius() override;
};

#endif
