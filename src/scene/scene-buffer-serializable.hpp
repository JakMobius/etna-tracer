#pragma once

#include "scene-renderer.hpp"
#include "buffer-chunk.hpp"

class SceneBufferSerializable;
class Scene;

class SceneBufferSerializable {

    int m_index_buffer_stride = 0;
    Scene* m_scene = nullptr;

public:

    void set_index_buffer_stride(int stride);
    int get_index_buffer_stride() const { return m_index_buffer_stride; }

    virtual void render(SceneRenderer& renderer, BufferChunk& chunk);

    Scene* get_scene() { return m_scene; }
    void set_scene(Scene* scene) { m_scene = scene; }
};