// implementation code for Polygon object class

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "Polygon.hpp"

// other classes used directly in the implementation
#include "Ray.hpp"
#include "Intersection.hpp"

void
Polygon::addVertex(const Vec3 &V)
{
    vertices.push_back(PolyVert(V));
}

void
Polygon::closePolygon()
{
    // compute normal from first two edges
    Vec3 V0 = vertices[0].V;
    Vec3 V1 = vertices[1].V;
    Vec3 V2 = vertices[2].V;
    Vec3 Vn = vertices.back().V;
    N = normalize(cross(V1 - V0, V2 - V0));

    // tangent and bitangent (2nd tangent perpendicular to 1st)
    // use edge between first and last vertex
    // avoids having to test that edge, and makes loops easier
    T = normalize(V0 - Vn);
    B = cross(N, T);

    // precomputed values for intersection testing
    V0_dot_N = dot(V0, N);
    for(auto &vert : vertices) {
        vert.Vt = dot(vert.V, T);
        vert.Vb = dot(vert.V, B);
    }
}

const Intersection
Polygon::intersect(const Ray &ray) const 
{
    // compute intersection point with plane
    float t = (V0_dot_N - dot(N, ray.E)) / dot(N, ray.D);

    if (t <= 0) return Intersection();  // behind ray start

    Vec3 P = ray.E + ray.D * t;

    // project P to onto plane basis vectors
    float Pt = dot(P, T), Pb = dot(P, B);

    // check if intersection is inside or outside
    // trace ray from p along a tangent vector and count even/odd intersections
    bool inside = false;
    for(auto v1 = vertices.begin(), v0 = v1++; v1 != vertices.end(); v0 = v1, ++v1) {
        // does edge straddle test ray where q dot bitangent = p dot bitangent?
        float b0 = v1->Vb - Pb, b1 = Pb - v0->Vb;
        if ((b0 > 0) ^ (b1 < 0)) {
            // outbound on test ray?
            float Qt = (b0 * v0->Vt + b1 * v1->Vt)/(v1->Vb - v0->Vb);
            if (Qt > Pt)
                inside = !inside;
        }
    }

    if (inside) return Intersection(this,t);

    return Intersection();          // no intersection
}
