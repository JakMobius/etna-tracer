#pragma once

#include "../obj/tiny_obj_loader.h"
#include "hittables/hittable-list.hpp"

Hittable& build_model(Scene& scene, const char* path, const Matrix4f& transform, Material& material);