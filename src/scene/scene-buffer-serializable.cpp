//
// Created by Артем on 02.10.2021.
//

#include "scene-buffer-serializable.hpp"
#include "scene.hpp"
#include "buffer-chunk.hpp"

void SceneBufferSerializable::set_index_buffer_stride(int stride) {
    m_index_buffer_stride = stride;
}

void SceneBufferSerializable::render(SceneRenderer&, BufferChunk&) {}
