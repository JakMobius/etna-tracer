#pragma once

#include "../utils/vec3.hpp"
#include "hittables/hittable-list.hpp"
#include "materials/material.hpp"

Hittable& build_rect(Scene& scene, Material& material, const Vec3f& center, const Vec3f& first_corner_offset, const Vec3f& second_corner_offset);