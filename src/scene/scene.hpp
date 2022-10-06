#pragma once

class Scene;
class HittableList;

#include <memory>
#include "hittables/hittable.hpp"
#include "scene-buffer.hpp"
#include "scene-renderer.hpp"
#include "scene-resource.hpp"

class Scene {
    std::vector<std::unique_ptr<SceneResource>> m_hittable_pool {};
    HittableList& m_root_hittable;
    SceneRenderer m_renderer;

public:
    Scene();
    ~Scene();

    HittableList& get_root_hittable() const { return m_root_hittable; }

    template<typename T, typename... Args>
    T& create(Args&&... args) {
        T* raw_ptr = new T(std::forward<Args>(args)...);
        auto ptr = std::unique_ptr<SceneResource>(static_cast<SceneResource*>(raw_ptr));
        m_hittable_pool.push_back(std::move(ptr));
        return *raw_ptr;
    }
};