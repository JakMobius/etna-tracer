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
    HittableList(): m_children() {
        set_index_buffer_stride(12);
    }

    void add_children(Hittable& hittable) {
        m_children.push_back(&hittable);
        // Two fields + children indices
        set_index_buffer_stride(12 + m_children.size());
    };

    void register_hittables(SceneRenderer& renderer) override {
        for(auto child : m_children) renderer.enqueue_hittable_render(*child);
    }

    void register_materials(SceneRenderer& renderer) override {
        for(auto child : m_children) child->register_materials(renderer);
    }

    void render(SceneRenderer& renderer, BufferChunk& chunk) override {
        int children_count = (int)m_children.size();

        chunk.write_index(HittableListType | (children_count << 3));

        for(int i = 0; i < children_count; i++) {
            int children_index = renderer.get_hittable_index(*m_children[i]);
            chunk.write_index(children_index);
        }

        AABB aabb = get_bounding_box();

        chunk.align();
        chunk.write_vector(aabb.lower);
        chunk.write_vector(aabb.upper);
    }

    void update_aabb() override {
        if(m_children.empty()) {
            bounding_box.lower = {0, 0, 0};
            bounding_box.upper = {0, 0, 0};
        } else {
            bounding_box = m_children[0]->get_bounding_box();
            for(int i = 1; i < (int)m_children.size(); i++) {
                bounding_box.extend(m_children[i]->get_bounding_box());
            }
        }
    }

    void flatten(std::vector<Hittable*>* storage) override {
        for(auto child : m_children) child->flatten(storage);
    }

    std::vector<Hittable*>& get_children() { return m_children; }
};