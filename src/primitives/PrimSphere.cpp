#include "PrimSphere.h"

#include <iostream>
#include <utility>

#include "Intersection.h"
#include "Ray.h"

bool PrimSphere::intersect(const Ray& ray, Intersection& out_hit) const {
    /**
     * NOTE: Ray is already transformed to the Model coordinate space.
     */
    using namespace glm;
    constexpr float kEpsilon = 1e-6f;

    /**
     * TODO(Task 2.1):
     * Implement ray-sphere intersection and write the nearest valid hit to `out_hit`.
     *
     * Suggested steps:
     * 1) Build quadratic coefficients for sphere equation.
     * 2) Compute discriminant and reject invalid cases.
     * 3) Pick nearest positive t (front-facing hit only).
     * 4) Compute hit point and unit normal.
     *
     * Once you compute a valid intersection, populate `out_hit` as:
     * out_hit = {t, point, normal, const_cast<PrimSphere *>(this), nullptr};
     *
     * Note:
     * - Model pointer should remain nullptr here; ModelBase assigns it later.
     * - Only accept intersections with t > kEpsilon.
     */

    vec3 oc = ray.p0 - this->center; 
    float a = dot(ray.dir, ray.dir);
    float b = 2.0f * dot(oc, ray.dir);
    float c = dot(oc, oc) - (this->radius * this->radius);

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) {
        return false;
    }

    float sqrt_discriminant = sqrt(discriminant);
    float t1 = (-b - sqrt_discriminant) / (2.0f * a);
    float t2 = (-b + sqrt_discriminant) / (2.0f * a);

    float t = t1;
    if (t <= kEpsilon) {
        t = t2;
        if (t <= kEpsilon) {
            return false;
        }
    }

    vec3 hit_point = ray.p0 + t * ray.dir;
    vec3 hit_normal = normalize(hit_point - this->center);
    out_hit = {t, hit_point, hit_normal, const_cast<PrimSphere *>(this), nullptr};

    return true;
}