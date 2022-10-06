//
// Created by Артем on 01.10.2021.
//

#include "hittable-list.hpp"
#include "bvh/bvh-node.hpp"

const int HittableListType = 0;

HittableList::HittableList() : m_children() {
    set_index_buffer_stride(12);
}

void HittableList::add_child(Hittable &hittable) {
    m_children.push_back(&hittable);
    // Two fields + children indices
    set_index_buffer_stride(12 + m_children.size());
}

void HittableList::render(SceneRenderer &renderer, BufferChunk &chunk) {
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

void HittableList::update_aabb() {
    if(m_children.empty()) {
        m_bounding_box.lower = {0, 0, 0};
        m_bounding_box.upper = {0, 0, 0};
    } else {
        m_bounding_box = m_children[0]->get_bounding_box();
        for(int i = 1; i < (int)m_children.size(); i++) {
            m_bounding_box.extend(m_children[i]->get_bounding_box());
        }
    }
}
