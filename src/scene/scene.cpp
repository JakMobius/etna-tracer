//
// Created by Артем on 01.10.2021.
//

#include "scene.hpp"
#include "hittables/hittable-list.hpp"
#include "scene-buffer.hpp"

Scene::Scene(): m_root_hittable(create<HittableList>()) {

}

Scene::~Scene() {

}
