// implementation code for Intersection class

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "Intersection.hpp"

// other classes used directly in the implementation
#include "Object.hpp"
#include "World.hpp"


// new intersection with object and intersection location
Intersection::Intersection(const Object *_obj, float _t) {
    t = _t;
    obj = _obj;
}

// return color for one intersection
const Vec3 
Intersection::color(const World &w, const Ray &r) const {
    if (obj)
        return obj->color(w, r, t);
    else
        // background color
        return w.background;
}
