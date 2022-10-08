//
// Created by Артем on 02.10.2021.
//

#include "scene-renderer.hpp"
#include "materials/material.hpp"
#include "hittables/hittable.hpp"
#include "hittables/hittable-list.hpp"
#include "hittables/bvh/bvh-tree.hpp"

SceneRenderer::SceneRenderer() : m_material_map(), m_hittable_map() {}

void SceneRenderer::allocate_buffers(SceneBufferSerializable& serializable) {
    int index_stride = serializable.get_index_buffer_stride();

    m_current_block_lengths.index_buffer_position = align(m_current_block_lengths.index_buffer_position + index_stride);
}

void SceneRenderer::enqueue_hittable_render(Hittable& hittable) {
    m_hittable_render_queue.push(&hittable);

    m_hittable_map.insert({&hittable, m_current_block_lengths });

    allocate_buffers(hittable);
}

void SceneRenderer::render_single(SceneBuffer& buffer, SceneBufferSerializable& serializable, SerializableChunkData& bounds) {
    BufferChunk chunk(
        buffer,
        bounds.index_buffer_position,
        serializable.get_index_buffer_stride()
    );
    serializable.render(*this, chunk);
}

void SceneRenderer::render(SceneBuffer& buffer, Scene& scene) {
    layout(scene);

    buffer.require_index_buffer_capacity(m_current_block_lengths.index_buffer_position);

    m_scene_buffer = &buffer;

    buffer.set_entry_hittable_index(m_material_block_length.index_buffer_position / m_alignment);

    for(auto entry : m_material_map) render_single(buffer, *entry.first, entry.second);
    for(auto entry : m_hittable_map) render_single(buffer, *entry.first, entry.second);

    m_scene_buffer = nullptr;
}

void SceneRenderer::layout(Scene& scene) {
    m_bvh_root = std::make_unique<BVHTree>(scene.get_root_hittable());

    m_hittable_map.clear();
    m_material_map.clear();

    m_material_block_length.reset();
    m_current_block_lengths.reset();

    while(!m_hittable_render_queue.empty()) {
        m_hittable_render_queue.pop();
    }

    m_bvh_root->register_materials(*this);

    m_material_block_length = m_current_block_lengths;

    enqueue_hittable_render(*m_bvh_root);

    while(!m_hittable_render_queue.empty()) {
        Hittable* next = m_hittable_render_queue.front();
        m_hittable_render_queue.pop();

        next->register_hittables(*this);
    }
}

void SceneRenderer::register_material(Material& material) {
    if(m_material_map.find(&material) == m_material_map.end()) {
        m_material_map.insert({&material, m_current_block_lengths});

        allocate_buffers(material);
    }
}
