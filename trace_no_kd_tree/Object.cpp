// implementation code for Object parent class

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "Object.hpp"

// default constructor just uses default color
Object::Object() {}

// construct object given color
Object::Object(const Surface &_color) {
    color = _color; 
}

// virtual destructor since this class has virtual members and derived children
Object::~Object() {}
