#pragma once

class SceneRenderer;
class Material;
class Hittable;
class Scene;
class SceneBufferSerializable;

#include <vector>
#include <memory>
#include <queue>
#include <set>
#include <map>
#include "scene-buffer.hpp"
#include "scene-buffer-serializable.hpp"

struct SerializableChunkData {
    int index_buffer_position;

    void reset() {
        index_buffer_position = 0;
    }
};

class SceneRenderer {
    SceneBuffer* m_scene_buffer;
    std::unique_ptr<Hittable> m_bvh_root;

    std::map<Material*, SerializableChunkData> m_material_map;
    std::map<Hittable*, SerializableChunkData> m_hittable_map;

    std::queue<Hittable*> m_hittable_render_queue;

    SerializableChunkData m_material_block_length = {-1 };
    SerializableChunkData m_current_block_lengths = {-1 };

    int m_alignment = 4;

    int align(int size) const {
        if(size % m_alignment > 0) {
            size += m_alignment - (size % m_alignment);
        }
        return size;
    }

    void layout(Scene& target);

public:
    explicit SceneRenderer();

    void register_material(Material& material);

    void enqueue_hittable_render(Hittable& hittable);

    int get_material_index(Material& material) { return m_material_map.at(&material).index_buffer_position / m_alignment; }
    int get_hittable_index(Hittable& hittable) { return m_hittable_map.at(&hittable).index_buffer_position / m_alignment; }

    void render(SceneBuffer& buffer, Scene& scene);

    SceneBuffer* get_scene_buffer() { return m_scene_buffer; }

    void render_single(SceneBuffer& buffer, SceneBufferSerializable& serializable, SerializableChunkData& bounds);

    void allocate_buffers(SceneBufferSerializable& serializable);
};