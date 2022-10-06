
#include "build-cube.hpp"
#include "hittables/hittable-list.hpp"
#include "hittables/hittable-triangle.hpp"
#include "../obj/tiny_obj_loader.h"

Hittable& build_cube(Scene& scene, const Matrix4f& transform, Material& material, CubeEdges bitmap) {

    Material* array[6] = {
        (bitmap & CubeEdges::x_pos) == CubeEdges::none ? nullptr : &material,
        (bitmap & CubeEdges::x_neg) == CubeEdges::none ? nullptr : &material,
        (bitmap & CubeEdges::y_pos) == CubeEdges::none ? nullptr : &material,
        (bitmap & CubeEdges::y_neg) == CubeEdges::none ? nullptr : &material,
        (bitmap & CubeEdges::z_pos) == CubeEdges::none ? nullptr : &material,
        (bitmap & CubeEdges::z_neg) == CubeEdges::none ? nullptr : &material,
    };

    return build_cube(scene, transform, array);
}

Hittable& build_cube(Scene& scene, const Matrix4f& transform, Material* materials[6]) {

    std::vector<Vec3f> vertices {
            {+0.5, +0.5, -0.5},
            {+0.5, -0.5, -0.5},
            {-0.5, +0.5, -0.5},
            {-0.5, -0.5, -0.5},
            {+0.5, +0.5, +0.5},
            {+0.5, -0.5, +0.5},
            {-0.5, +0.5, +0.5},
            {-0.5, -0.5, +0.5}
    };

    for(auto& vertex : vertices) vertex *= transform;

    auto& result = scene.create<HittableList>();

    if(materials[0]) {
        result.add_children(scene.create<HittableTriangle>(vertices[0], vertices[5], vertices[1], *materials[0]));
        result.add_children(scene.create<HittableTriangle>(vertices[0], vertices[4], vertices[5], *materials[0]));
    }

    if(materials[1]) {
        result.add_children(scene.create<HittableTriangle>(vertices[2], vertices[7], vertices[6], *materials[1]));
        result.add_children(scene.create<HittableTriangle>(vertices[2], vertices[3], vertices[7], *materials[1]));
    }

    if(materials[2]) {
        result.add_children(scene.create<HittableTriangle>(vertices[0], vertices[2], vertices[6], *materials[2]));
        result.add_children(scene.create<HittableTriangle>(vertices[0], vertices[6], vertices[4], *materials[2]));
    }

    if(materials[3]) {
        result.add_children(scene.create<HittableTriangle>(vertices[1], vertices[7], vertices[3], *materials[3]));
        result.add_children(scene.create<HittableTriangle>(vertices[1], vertices[5], vertices[7], *materials[3]));
    }

    if(materials[4]) {
        result.add_children(scene.create<HittableTriangle>(vertices[4], vertices[7], vertices[5], *materials[4]));
        result.add_children(scene.create<HittableTriangle>(vertices[4], vertices[6], vertices[7], *materials[4]));
    }

    if(materials[5]) {
        result.add_children(scene.create<HittableTriangle>(vertices[0], vertices[1], vertices[3], *materials[5]));
        result.add_children(scene.create<HittableTriangle>(vertices[0], vertices[3], vertices[2], *materials[5]));
    }

    return result;
}