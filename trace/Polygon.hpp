// polygon objects
#ifndef POLYGON_HPP
#define POLYGON_HPP

// other classes we use DIRECTLY in our interface
#include "Object.hpp"
#include "Vec3.hpp"

// system includes necessary for the interface
#include <vector>

// classes we only use by pointer or reference
class Appearance;
class World;
class Ray;

class Polygon : public Object {
private: // private data
    struct PolyVert {
        Vec3 V;              // vertex location

        // derived, for intersection testing
        float Vt, Vb;   // coordinates in basis
        
        PolyVert(const Vec3 _V) { V = _V; }
    };
    typedef std::vector<PolyVert> VertexList;

    VertexList vertices;    // list of vertices
    Vec3 N;                 // face normal
    Vec3 T;                 // first basis vector in polygon plane
    Vec3 B;                 // 2nd basis vector in polygon plane

    // derived, for intersection testing
    float V0_dot_N;

public: // constructors
    Polygon(const Surface &_surface) : Object(_surface) {}

public: // manipulators
    // add a new vertex to the polygon
    // given vertex and per-vertex normal
    void addVertex(const Vec3 v);

    // close the polygon after the last vertex
    void closePolygon();

public: // object functions
    const Intersection intersect(const Ray &ray) const override;
    const Vec3 normal(const Vec3 P) const override;
};

#endif
