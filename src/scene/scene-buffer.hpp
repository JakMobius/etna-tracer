#pragma once

#include <vector>
#include "../utils/vec3.hpp"

class SceneBuffer {
    std::vector<int> scene_index_buffer;
    int entry_hittable_index = 0;

public:
    SceneBuffer() {
    }

    int get_entry_hittable_index() const { return entry_hittable_index; }
    void set_entry_hittable_index(int p_entry_hittable_index) { entry_hittable_index = p_entry_hittable_index; }

    std::vector<int>& get_index_buffer() { return scene_index_buffer; }
    const std::vector<int>& get_index_buffer() const { return scene_index_buffer; }

    void require_index_buffer_capacity(int capacity) {
        if((int)scene_index_buffer.size() < capacity) {
            scene_index_buffer.resize(capacity);
        }
    }
};