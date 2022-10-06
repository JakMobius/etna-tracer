#pragma once

#include <cassert>
#include "scene-buffer.hpp"

class BufferChunk {
    SceneBuffer& m_buffer;

    int m_index_buffer_position;

    int m_index_buffer_length;

    int m_internal_index_position = 0;

    std::vector<int>& get_index_buffer() { return m_buffer.get_index_buffer(); }

public:

    BufferChunk(SceneBuffer& buffer, int index_buffer_position, int index_buffer_length):
            m_buffer(buffer),
            m_index_buffer_position(index_buffer_position),
            m_index_buffer_length(index_buffer_length) {}

    void write_index(int index) {
        assert(m_internal_index_position < m_index_buffer_length);
        get_index_buffer()[m_index_buffer_position + m_internal_index_position++] = index;
    }

    void write_float(float flt) {
        write_index(*((int*)&flt));
    }

    void align() {
        while(m_internal_index_position % 4) write_index(0);
    }

    void write_vector(const Vec3f& vector, bool skip_fourth_component = true) {
        write_float(vector[0]);
        write_float(vector[1]);
        write_float(vector[2]);
        if(skip_fourth_component) m_internal_index_position++;
    };
};