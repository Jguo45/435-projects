// implementation code for ObjectList class
// list of objects in scene

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "ObjectList.hpp"
#include "Object.hpp"

// delete all objects in the list
ObjectList::~ObjectList() {
    for(auto obj : objects)
        delete obj;
}

// trace ray r through all objects, returning first intersection
const Intersection
ObjectList::trace(Ray r) const
{
    Intersection closest;       // no object, t = infinity
    for(auto obj : objects) {
        Intersection current = obj->intersect(r);
        if (current < closest)
            closest = current;
    }
    return closest;
}

const bool ObjectList::probe(Ray r, float distance) const {
    for (auto obj : objects) {
        bool intersect = obj->probe(r, distance);
        if (intersect)
            return false;
    }
    return true;
}