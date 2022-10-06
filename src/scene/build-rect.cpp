//
// Created by Артем on 02.10.2021.
//

#include "build-rect.hpp"
#include "hittables/hittable-list.hpp"
#include "hittables/hittable-triangle.hpp"

Hittable& build_rect(Scene& scene, Material& material, const Vec3f& center, const Vec3f& first_corner_offset, const Vec3f& second_corner_offset) {
    auto& result = scene.create<HittableList>();

    result.add_children(scene.create<HittableTriangle>(center + first_corner_offset, center + second_corner_offset, center - first_corner_offset, material));
    result.add_children(scene.create<HittableTriangle>(center + first_corner_offset, center - first_corner_offset, center - second_corner_offset, material));

    return result;
}