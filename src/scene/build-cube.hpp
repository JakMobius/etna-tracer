#pragma once

#include "build-cube.hpp"
#include "../utils/vec3.hpp"
#include "scene.hpp"
#include <string>

enum class CubeEdges : int {
    none = 0,
    x_pos = (1 << 0),
    x_neg = (1 << 1),
    y_pos = (1 << 2),
    y_neg = (1 << 3),
    z_pos = (1 << 4),
    z_neg = (1 << 5),
    all = x_pos | x_neg | y_pos | y_neg | z_pos | z_neg
};

inline CubeEdges operator|(CubeEdges a, CubeEdges b) { return static_cast<CubeEdges>(static_cast<int>(a) | static_cast<int>(b)); }
inline CubeEdges operator&(CubeEdges a, CubeEdges b) { return static_cast<CubeEdges>(static_cast<int>(a) & static_cast<int>(b)); }

Hittable& build_cube(Scene& scene, const Matrix4f& transform, Material& material, CubeEdges bitmap = CubeEdges::all);
Hittable& build_cube(Scene& scene, const Matrix4f& transform, Material* materials[6]);