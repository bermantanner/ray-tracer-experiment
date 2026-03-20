#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "PrimTriangle.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "Intersection.h"
#include "Ray.h"

PrimTriangle::PrimTriangle(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals) {
    this->vertices[0] = vertices[0];
    this->vertices[1] = vertices[1];
    this->vertices[2] = vertices[2];

    this->normals[0] = normals[0];
    this->normals[1] = normals[1];
    this->normals[2] = normals[2];
}

bool PrimTriangle::intersect(const Ray& ray, Intersection& out_hit) const {
    using namespace glm;
    /**
     * NOTE: Ray is already transformed to the Model coordinate space.
     */
    constexpr float kEpsilon = 1e-6f;


    vec3 v0 = this->vertices[0];
    vec3 v1 = this->vertices[1];
    vec3 v2 = this->vertices[2];

    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    vec3 h = cross(ray.dir, edge2);
    float a = dot(edge1, h);

    if (a > -kEpsilon && a < kEpsilon) {
        return false; 
    }

    float f = 1.0f / a;
    vec3 s = ray.p0 - v0;

    float u = f * dot(s, h);
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.dir, q);
    
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    float t = f * dot(edge2, q);
    
    if (t > kEpsilon) {
        vec3 hit_point = ray.p0 + ray.dir * t;
        
        vec3 n0 = this->normals[0];
        vec3 n1 = this->normals[1];
        vec3 n2 = this->normals[2];
        
        vec3 interpolated_normal = normalize((1.0f - u - v) * n0 + u * n1 + v * n2);

        out_hit = {t, hit_point, interpolated_normal, const_cast<PrimTriangle *>(this), nullptr};
        return true;
    }

    return false;
}