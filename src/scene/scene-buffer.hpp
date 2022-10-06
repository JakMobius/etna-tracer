#pragma once

#include <vector>
#include "../utils/vec3.hpp"

class SceneBuffer {
    std::vector<int> m_scene_index_buffer;
    int m_entry_hittable_index = 0;

public:
    SceneBuffer() {
    }

    int get_entry_hittable_index() const { return m_entry_hittable_index; }
    void set_entry_hittable_index(int entry_hittable_index) { m_entry_hittable_index = entry_hittable_index; }

    std::vector<int>& get_index_buffer() { return m_scene_index_buffer; }
    const std::vector<int>& get_index_buffer() const { return m_scene_index_buffer; }

    void require_index_buffer_capacity(int capacity) {
        if((int)m_scene_index_buffer.size() < capacity) {
            m_scene_index_buffer.resize(capacity);
        }
    }
};