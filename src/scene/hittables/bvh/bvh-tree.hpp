#pragma once

class BVHTree;

#include <vector>
#include "bvh-node.hpp"

class BVHTree : public Hittable {
    std::vector<BVHNode> m_nodes;
    std::vector<Hittable *> m_leaves;
    int m_stride = 0;

public:
    explicit BVHTree(Hittable &hittable);

    void register_hittables(SceneRenderer &renderer) override {
        for (auto leaf: m_leaves) renderer.enqueue_hittable_render(*leaf);
    }

    void register_materials(SceneRenderer &renderer) override {
        for (auto leaf: m_leaves) leaf->register_materials(renderer);
    }

    void render(SceneRenderer &renderer, BufferChunk &chunk) override;

    BVHNode *get_node(int index);

    void update_aabb() override;

    void generate_bvh(int node_index, int from, int to, std::vector<Hittable *> *list);
};
