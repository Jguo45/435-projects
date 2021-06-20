// implementation code for Object parent class

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "Object.hpp"
#include "World.hpp"

// default constructor just uses default color
Object::Object() {}

Object::Object(const Object& rhs) {
    surface = rhs.surface;
}

// construct object given surface appearance
Object::Object(const Surface &_surface) {
    surface = _surface; 
}

// virtual destructor since this class has virtual members and derived children
Object::~Object() {}

// shared surface color computation for all object types
// Color of this object
const Vec3 Object::color(const World &world, const Ray &ray, float t) const
{
    // base color
    Vec3 col(0,0,0);

    if ((World::effects & World::AMBIENT))
        col = surface.ambient;

    // view ray
    Vec3 V = -normalize(ray.D);

    // position and normal at intersection
    Vec3 P = ray.E + t * ray.D;
    Vec3 N = normal(P);

    // diffuse and specular
    for (auto li : world.lights) {

        Vec3 L = li.pos - P;   // light vector
        float LLen = length(L);
        L = L / LLen;

        float N_dot_L = dot(N,L);

        // check for negative dot product first to avoid shadow cast
        if (N_dot_L > 0) {

            // cast ray to see if it's in shadow
            if (! (World::effects & World::SHADOW) || 
                ! world.objects->probe(Ray(P, L, 1e-4f, LLen))) {

                if (World::effects & World::DIFFUSE)
                    col = col + li.col * surface.diffuse * N_dot_L;

                if ((World::effects & World::SPECULAR) && 
                    surface.specular[0]+surface.specular[1]+surface.specular[2] > 0.f) {

                    // normalized L and H
                    Vec3 H = normalize(V+L);

                    float N_dot_H = dot(N,H);
                    if (N_dot_H > 0)
                        col = col + li.col * surface.specular * pow(N_dot_H, surface.e);
                }
            }
        }
    }

    // reflected rays
    if ((World::effects & World::REFLECT) &&
        ray.influence * surface.kr > world.cutoff && ray.bounces > 0) {

        // reflect ray off surface
        Vec3 rv = ray.D - 2*dot(N, ray.D)*N;

        // new ray with one less bounce and influence reduced by kr
        Ray rr(P, rv, 1e-4f, INFINITY, ray.bounces-1, ray.influence*surface.kr);
        Vec3 rc = world.objects->trace(rr).color(world,rr); // trace ray
        col = col + surface.kr * rc;
    }

    // refracted rays
    if ((World::effects & World::REFRACT) &&
            ray.influence * surface.kt > world.cutoff && ray.bounces > 0) {

        // compute refracted ray
        float ci = dot(N,V);                // cosine of incident ray angle
        float tir = ci > 0 ? 1/surface.ir : surface.ir;     // ratio of air to object or object to air
        float ct2 = 1-(1-ci*ci)*tir*tir;    // cosine squared of refracted ray
        if (ct2 > 0) {                  // <=0 for total internal reflection
            // ray direction
            Vec3 td;
            if (ci>0)                   // into surface
                td = N*(ci*tir - sqrtf(ct2)) - V*tir;
            else                        // out of surface
                td = N*(ci*tir + sqrtf(ct2)) - V*tir;

            // new ray with one fewer bounce and influence reduced by kt
            Ray tr(P, td, 1e-4f, INFINITY, ray.bounces-1, ray.influence*surface.kt);
            Vec3 tc = world.objects->trace(tr).color(world,tr); // trace ray
            col = col + surface.kt * tc;
        }
    }

    return col;
}
