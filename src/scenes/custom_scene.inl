#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "DiffuseMaterial.h"
#include "MirrorMaterial.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Square.h"
#include "Utility.h"

using namespace glm;

namespace {

// helper function for building 1x1x1 cuboid node out of 6 squares
std::unique_ptr<Node> create_cuboid(std::shared_ptr<DiffuseMaterial> mat) {
    std::unique_ptr<Node> cuboid_node = std::make_unique<Node>();

    // pos X
    std::unique_ptr<Node> face1 = std::make_unique<Node>();
    face1->model = std::make_unique<Square>(vec3(0.0f), 1.0f, vec3(1.0f, 0.0f, 0.0f), mat);
    cuboid_node->childnodes.push_back(std::move(face1));
    cuboid_node->childtransforms.push_back(translate(vec3(0.5f, 0.0f, 0.0f)));

    // neg X
    std::unique_ptr<Node> face2 = std::make_unique<Node>();
    face2->model = std::make_unique<Square>(vec3(0.0f), 1.0f, vec3(-1.0f, 0.0f, 0.0f), mat);
    cuboid_node->childnodes.push_back(std::move(face2));
    cuboid_node->childtransforms.push_back(translate(vec3(-0.5f, 0.0f, 0.0f)));

    // pos Y
    std::unique_ptr<Node> face3 = std::make_unique<Node>();
    face3->model = std::make_unique<Square>(vec3(0.0f), 1.0f, vec3(0.0f, 1.0f, 0.0f), mat);
    cuboid_node->childnodes.push_back(std::move(face3));
    cuboid_node->childtransforms.push_back(translate(vec3(0.0f, 0.5f, 0.0f)));

    // neg Y
    std::unique_ptr<Node> face4 = std::make_unique<Node>();
    face4->model = std::make_unique<Square>(vec3(0.0f), 1.0f, vec3(0.0f, -1.0f, 0.0f), mat);
    cuboid_node->childnodes.push_back(std::move(face4));
    cuboid_node->childtransforms.push_back(translate(vec3(0.0f, -0.5f, 0.0f)));

    // pos Z
    std::unique_ptr<Node> face5 = std::make_unique<Node>();
    face5->model = std::make_unique<Square>(vec3(0.0f), 1.0f, vec3(0.0f, 0.0f, 1.0f), mat);
    cuboid_node->childnodes.push_back(std::move(face5));
    cuboid_node->childtransforms.push_back(translate(vec3(0.0f, 0.0f, 0.5f)));

    // neg Z
    std::unique_ptr<Node> face6 = std::make_unique<Node>();
    face6->model = std::make_unique<Square>(vec3(0.0f), 1.0f, vec3(0.0f, 0.0f, -1.0f), mat);
    cuboid_node->childnodes.push_back(std::move(face6));
    cuboid_node->childtransforms.push_back(translate(vec3(0.0f, 0.0f, -0.5f)));

    return cuboid_node;
}

Scene* build_custom_scene() {
    // materials
    std::shared_ptr<DiffuseMaterial> diffuse_wall = std::make_shared<DiffuseMaterial>(LinearToSRGB(vec3(0.2f, 0.2f, 0.2f))); // Darker walls for mood
    std::shared_ptr<DiffuseMaterial> diffuse_pillar = std::make_shared<DiffuseMaterial>(LinearToSRGB(vec3(0.1f, 0.1f, 0.1f))); // Almost black pillars
    
    // NEW: mirror material for the side walls
    std::shared_ptr<MirrorMaterial> mirror_wall = std::make_shared<MirrorMaterial>(vec3(0.95f)); 
    
    // backlight detail
    std::shared_ptr<DiffuseMaterial> light_material = std::make_shared<DiffuseMaterial>(vec3(0.0f));
    light_material->convert_to_light(vec3(0.2f, 0.6f, 1.0f), vec3(80.0f)); 

    std::unique_ptr<Node> root_node = std::make_unique<Node>();

    // room
    // Floor
    std::unique_ptr<Node> floor_square = std::make_unique<Node>();
    floor_square->model = std::make_unique<Square>(vec3(0.0f), 4.0f, vec3(0.0f, 1.0f, 0.0f), diffuse_wall);
    root_node->childnodes.push_back(std::move(floor_square));
    // NEW: Scaled by 4 on the Z axis, shifted forward to z = 6.0
    root_node->childtransforms.push_back(translate(vec3(0.0f, -2.0f, 6.0f)) * scale(vec3(1.0f, 1.0f, 4.0f)));

    // Ceiling
    std::unique_ptr<Node> ceiling_square = std::make_unique<Node>();
    ceiling_square->model = std::make_unique<Square>(vec3(0.0f), 4.0f, vec3(0.0f, -1.0f, 0.0f), diffuse_wall);
    root_node->childnodes.push_back(std::move(ceiling_square));
    // NEW: Scaled by 4 on the Z axis, shifted forward to z = 6.0
    root_node->childtransforms.push_back(translate(vec3(0.0f, 2.0f, 6.0f)) * scale(vec3(1.0f, 1.0f, 4.0f)));

    // Left Wall (Mirror)
    std::unique_ptr<Node> left_square = std::make_unique<Node>();
    left_square->model = std::make_unique<Square>(vec3(0.0f), 4.0f, vec3(1.0f, 0.0f, 0.0f), mirror_wall);
    root_node->childnodes.push_back(std::move(left_square));
    // NEW: Scaled by 4 on the Z axis, shifted forward to z = 6.0
    root_node->childtransforms.push_back(translate(vec3(-2.0f, 0.0f, 6.0f)) * scale(vec3(1.0f, 1.0f, 4.0f)));

    // Right Wall (Mirror)
    std::unique_ptr<Node> right_square = std::make_unique<Node>();
    right_square->model = std::make_unique<Square>(vec3(0.0f), 4.0f, vec3(-1.0f, 0.0f, 0.0f), mirror_wall);
    root_node->childnodes.push_back(std::move(right_square));
    // NEW: Scaled by 4 on the Z axis, shifted forward to z = 6.0
    root_node->childtransforms.push_back(translate(vec3(2.0f, 0.0f, 6.0f)) * scale(vec3(1.0f, 1.0f, 4.0f)));

    std::unique_ptr<Node> back_square = std::make_unique<Node>();
    back_square->model = std::make_unique<Square>(vec3(0.0f), 4.0f, vec3(0.0f, 0.0f, 1.0f), diffuse_wall);
    root_node->childnodes.push_back(std::move(back_square));
    root_node->childtransforms.push_back(translate(vec3(0.0f, 0.0f, -2.0f)));

    // backlight placing
    std::unique_ptr<Node> square_light = std::make_unique<Node>();
    square_light->model = std::make_unique<Square>(vec3(0.0f), 2.0f, vec3(0.0f, 0.0f, 1.0f), light_material);
    root_node->childnodes.push_back(std::move(square_light));
    root_node->childtransforms.push_back(translate(vec3(0.0f, -0.5f, -1.9f)));

    // stretched pillars
    int num_pillars = 25; // Drastically increase the number of pillars
    for (int i = 0; i < num_pillars; i++) {
        std::unique_ptr<Node> pillar = create_cuboid(diffuse_pillar);
        
        // Scatter randomly across the width of the room
        float x_offset = rand_uniform(-1.8f, 1.8f); 
        
        // Scatter randomly in depth, keeping them between the camera and the backlight (-1.9f)
        float z_offset = rand_uniform(-1.5f, 0.8f); 
        
        // Randomize the thickness of each pillar for variety
        float thickness = rand_uniform(0.05f, 0.25f);
        
        // Randomize the rotation
        float rot_angle = rand_uniform(0.0f, 360.0f);
        
        glm::mat4 scale_matrix = scale(vec3(thickness, 4.0f, thickness));
        glm::mat4 translate_matrix = translate(vec3(x_offset, 0.0f, z_offset));
        glm::mat4 rotate_matrix = rotate(degree_to_rad(rot_angle), vec3(0.0f, 1.0f, 0.0f));

        root_node->childnodes.push_back(std::move(pillar));
        root_node->childtransforms.push_back(translate_matrix * rotate_matrix * scale_matrix);
    }

    return new Scene(std::move(root_node));
}

}  // namespace

Scene* custom_scene() {
    return build_custom_scene();
}