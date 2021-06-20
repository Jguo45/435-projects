// implementation code for ObjectList class
// list of objects in scene

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "ObjectList.hpp"
#include "Object.hpp"
#include <iostream>
#include <atomic>

static std::atomic<int> RayCount(0), ShadowCount(0);

// delete list and objects it contains
ObjectList::~ObjectList() {
    std::cout << RayCount << " Photon Ray" << (RayCount == 1 ? "" : "s") << "; "
        << ShadowCount << " Shadow Ray" << (ShadowCount == 1 ? "" : "s") << '\n';
    for(auto obj : objects)
        delete obj;
}

const ObjectList& ObjectList::operator=(const ObjectList& rhs)
{
	if (this != &rhs) {
		for (auto obj : rhs.objects) {
			addObject(obj);
		}
	}

	return *this;
}

// trace ray r through all objects, returning first intersection
const Intersection
ObjectList::trace(Ray r) const
{
    ++RayCount;
    Intersection closest;       // no object, t = infinity
    for(auto obj : objects) {
        Intersection current = obj->intersect(r);
        if (current < closest)
            closest = current;
    }
    return closest;
}

// trace ray r through all objects, returning true if there is any
// intersection between r.near and r.far
const bool
ObjectList::probe(Ray r) const
{
    ++ShadowCount;
    for(auto obj : objects) {
        if (obj->intersect(r).t < r.far)
            return true;
    }
    return false;
}

int ObjectList::determineSplitAxis(float& min, float& max) {
	float minValue[3];
	float maxValue[3];

	for (int i = 0; i < 3; i++) {
		Object* obj = objects[0];
		minValue[i] = obj->getCenter()[i] - obj->getRadius();
		maxValue[i] = obj->getCenter()[i] + obj->getRadius();
	}

	for (auto obj : objects) {
		Vec3 center = obj->getCenter();
		float radius = obj->getRadius();

		for (int i = 0; i < 3; i++) {
			minValue[i] = std::min(minValue[i], center[i] - radius);
			maxValue[i] = std::max(maxValue[i], center[i] + radius);
		}
	}

	int axis = 0;
	float range = maxValue[0] - minValue[0];
	for (int i = 1; i < 3; i++) {
		float tmpRange = maxValue[i] - minValue[i];
		if (tmpRange > range) {
			range = tmpRange;
			axis = i;
		}
	}

	min = minValue[axis];
	max = maxValue[axis];

	return axis;
}