// virtual class for any object
#ifndef OBJECT_HPP
#define OBJECT_HPP

// other classes we use DIRECTLY in our interface
#include "Intersection.hpp"
#include "Vec3.hpp"

// classes we only use by pointer or reference
class World;
class Ray;

// collected surface appearance parameters
struct Surface {
    Vec3 ambient;   // ambient color
    Vec3 diffuse;   // diffuse color
    Vec3 specular;  // specular color
    float e;        // specular coefficient
    float kr, kt, ir;   // reflection and transmission coeffients & index of refraction

    Surface() : ambient(0,0,0), diffuse(1,1,1), specular(0,0,0), e(0), kr(0), kt(0), ir(1) {}
};

class Object {
public: // data visible to children
    Surface surface;        // this object's appearance parameters

public: // constructor & destructor
    Object();
    Object(const Object& rhs);
    Object(const Surface &_surface);
    virtual ~Object();


public: // computational members
    // return t for closest intersection with ray
    virtual const Intersection intersect(const Ray &ray) const = 0;

    // normal for at point P
    virtual const Vec3 normal(const Vec3 P) const = 0;

    virtual Vec3 getCenter() = 0;
    virtual float getRadius() = 0;

	// compute color at ray intersection
	const Vec3 color(const World &w, const Ray &r, float t) const;
};

#endif
