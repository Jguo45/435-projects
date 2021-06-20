// implementation code for Sphere object class

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "Sphere.hpp"

// other classes used directly in the implementation
#include "Ray.hpp"

Sphere::Sphere(const Surface &_color, const Vec3 &_center, float _radius)
    : Object(_color) 
{
    C = _center;
    R = _radius;
    Rsquared = R*R;
}

// sphere-ray intersection
const Intersection
Sphere::intersect(const Ray &r) const
{
    // solve p=r.start-center + t*r.direction; p*p -radius^2=0
    float a = r.D_dot_D;
    Vec3 g = r.E - C;
    float b = dot(r.D, g);
    float c = dot(g,g) - Rsquared;

    float discriminant = b*b - a*c;
    if (discriminant < 0)       // no intersection
        return Intersection();

    // solve quadratic equation for desired surface
    float dsq = sqrtf(discriminant);
    float t = (-b - dsq) / a;       // first intersection within ray extent?
    if (t > BIAS) 
        return Intersection(this,t);

    t = (-b + dsq) / a;             // second intersection within ray extent?
    if (t > BIAS) 
        return Intersection(this,t);

    return Intersection();              // sphere entirely behind start point
}

const Vec3 Sphere::getCenter() const {
    return C;
}

bool Sphere::probe(const Ray& r, float distance) {
    float a = r.D_dot_D;
    Vec3 g = r.E - C;
    float b = dot(r.D, g);
    float c = dot(g, g) - Rsquared;

    float discriminant = b * b - a * c;
    
    if (discriminant < 0) {
        return false;
    }
    else {
        float dsq = sqrtf(discriminant);
        float t = (-b - dsq) / a;
        if (BIAS <= t && t <= distance)
            return true;
        t = (-b + dsq) / a;
        if (BIAS <= t && t <= distance)
            return true;

        return false;
    }
}


