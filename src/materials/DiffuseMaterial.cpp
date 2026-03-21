#define GLM_ENABLE_EXPERIMENTAL
#include "DiffuseMaterial.h"

#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <limits>
#include <mutex>

#include "ModelBase.h"
#include "Scene.h"

using namespace glm;

namespace {
constexpr float kRayEpsilon = 0.001f;
constexpr int kDirectLightSamples = 8;
constexpr float kPi = 3.14159265358979323846f;
}  // namespace

Ray DiffuseMaterial::sample_ray_and_update_radiance(Ray& ray, Intersection& intersection) {
    /**
     * Calculate the next ray after intersection with the model.
     * This will be used for recursive ray tracing.
     */

    vec3 normal = intersection.normal;
    vec3 point = intersection.point;

    // Diffuse reflection

    float s = rand01();
    float t = rand01();

    float u = 2.0f * kPi * s;
    float v = std::sqrt(1.0f - t);

    vec3 hemisphere_sample = vec3(v * std::cos(u), std::sqrt(t), v * std::sin(u));

    vec3 new_dir = align_with_normal(hemisphere_sample, normal);

    vec3 W_diffuse = this->albedo;

    ray.W_wip = ray.W_wip * W_diffuse;
    ray.p0 = point + kRayEpsilon * normal;
    ray.dir = new_dir;
    ray.allow_emissive_hit = false;  // old `is_diffuse_bounce=true` policy mapping
    ray.n_bounces++;

    return ray;
}

glm::vec3 DiffuseMaterial::get_direct_lighting(Intersection const& intersection, Scene const& scene) const {
    using namespace glm;

    vec3 cumulative_direct_light = vec3(0.0f);
    
    for (unsigned int idx = 0; idx < scene.light_sources.size(); idx++) {
        ModelBase* light_source = scene.light_sources[idx];

        if (light_source == intersection.model)
            continue;

        vec3 summed_samples = vec3(0.0f);
        
        for (int sample_idx = 0; sample_idx < kDirectLightSamples; sample_idx++) {
            vec3 light_pos = light_source->get_surface_point();
            vec3 to_light = light_pos - intersection.point;
            float dist2 = dot(to_light, to_light);
            if (dist2 <= 0.0f)
                continue;

            float dist = std::sqrt(dist2);
            vec3 light_dir = to_light / dist;

            float c_x = std::max(dot(intersection.normal, light_dir), 0.0f);
            if (c_x <= 0.0f)
                continue;

            Ray shadow_ray;
            shadow_ray.p0 = intersection.point + kRayEpsilon * intersection.normal; 
            shadow_ray.dir = light_dir; 

            Intersection shadow_hit;
            bool is_visible = false;
            
            if (!scene.intersect_nearest(shadow_ray, shadow_hit) || shadow_hit.model == light_source) {
                is_visible = true; 
            }

            if (is_visible) {
                vec3 light_normal = vec3(0.0f, -1.0f, 0.0f); 
                float c_y = std::max(dot(light_normal, -light_dir), 0.0f);

                float light_area = light_source->get_surface_area();
                vec3 emitted_radiance = light_source->material->emission / light_area; 
                vec3 direct_light = emitted_radiance * ( (c_x * c_y) / dist2 );
                
                summed_samples += direct_light;
            }
        }

        float light_area = light_source->get_surface_area();
        float normalization = (kDirectLightSamples > 0) ? (light_area / static_cast<float>(kDirectLightSamples)) : 0.0f;
        
        vec3 light_contribution = summed_samples * normalization; 
        
        cumulative_direct_light += light_contribution;
    }

    return cumulative_direct_light * albedo / kPi;
}

vec3 DiffuseMaterial::color_of_last_bounce(Ray& ray, Intersection& intersection, Scene const& scene) {
    using namespace glm;
    
    // Replace fallback with finished direct-light shading.
    vec3 direct_light = this->get_direct_lighting(intersection, scene);
    vec3 shaded = ray.W_wip * direct_light;
    
    return shaded; 
}
