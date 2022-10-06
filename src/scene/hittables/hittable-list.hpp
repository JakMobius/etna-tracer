#pragma once

class HittableList;
struct BVHNode;

#include <vector>
#include "hittable.hpp"
#include "../scene-renderer.hpp"
#include "../buffer-chunk.hpp"

extern const int HittableListType;

class HittableList : public Hittable {
    std::vector<Hittable*> m_children;

public:
    HittableList();

    void add_child(Hittable& hittable);;

    void register_hittables(SceneRenderer& renderer) override {
        for(auto child : m_children) renderer.enqueue_hittable_render(*child);
    }

    void register_materials(SceneRenderer& renderer) override {
        for(auto child : m_children) child->register_materials(renderer);
    }

    void render(SceneRenderer& renderer, BufferChunk& chunk) override;

    void update_aabb() override;

    void flatten(std::vector<Hittable*>* storage) override {
        for(auto child : m_children) child->flatten(storage);
    }

    std::vector<Hittable*>& get_children() { return m_children; }
};