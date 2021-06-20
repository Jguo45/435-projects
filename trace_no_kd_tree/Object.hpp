// virtual class for any object
#ifndef OBJECT_HPP
#define OBJECT_HPP

// other classes we use DIRECTLY in our interface
#include "Intersection.hpp"
#include "Vec3.hpp"

// classes we only use by pointer or reference
class World;
class Ray;

class Surface {
public:
    Surface() {
        _title = "Default";
        _ambient = Vec3();
        _diffuse = Vec3();
        _specular = Vec3();
        _specpow = 0;
        _reflect = 0;
    }

    Surface(std::string title) {
        _title = title;
        _ambient = Vec3();
        _diffuse = Vec3();
        _specular = Vec3();
        _specpow = 0;
        _reflect = 0;
    }

public:
    std::string _title;
    Vec3 _ambient;
    Vec3 _diffuse;
    Vec3 _specular;
    float _specpow;
    float _reflect;
};

class Object {
public: // public data
    Surface color;         // this object's color

public: // constructor & destructor
    Object();
    Object(const Surface &_color);
    virtual ~Object();


public: // computational members
    // return t for closest intersection with ray
    virtual const Intersection intersect(const Ray &ray) const = 0;
    virtual const Vec3 getCenter() const = 0;
    virtual bool probe(const Ray& ray, float distance) = 0;
};

#endif
