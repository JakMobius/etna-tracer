//
// Created by Артем on 13.10.2021.
//

#include "bvh-tree.hpp"

void BVHTree::generate_bvh(int node_index, int from, int to, std::vector<Hittable*>* list) {
    int count = to - from;

    BVHNode* node = get_node(node_index);

    if(count == 0) return;
    if(count == 1) {
        node->leaf = (*list)[from];
        node->flags |= BVHNodeFlags::is_leaf;
        node->bounding_box = node->leaf->get_bounding_box();
        return;
    }

    double est_x = node->get_effective_split_size(list, from, to, 0);
    double est_y = node->get_effective_split_size(list, from, to, 1);
    double est_z = node->get_effective_split_size(list, from, to, 2);

    int split_axis = 0;

    if (est_x <= est_y && est_x <= est_z) split_axis = 0;
    else if (est_y <= est_x && est_y <= est_z) split_axis = 1;
    else split_axis = 2;

    auto comparator = BVH_AABB_COMPARATORS[split_axis];

    std::sort(list->begin() + from, list->begin() + to, comparator);

    int split_index = from + count / 2;

    int left_child_index = node_index * 2 + 1;
    int right_child_index = node_index * 2 + 2;

    generate_bvh(left_child_index, from, split_index, list);
    generate_bvh(right_child_index, split_index, to, list);

    // In case internal storage was reallocated
    node = get_node(node_index);

    BVHNode* left = get_node(left_child_index);
    BVHNode* right = get_node(right_child_index);

    node->bounding_box = left->bounding_box;
    node->bounding_box.extend(right->bounding_box);

    for(int i = 0; i < 3; i++) {
        if(node->bounding_box.upper[i] == left->bounding_box.upper[i]) left->flags |= BVHNodeFlags::n_positive(i);
        if(node->bounding_box.upper[i] == right->bounding_box.upper[i]) right->flags |= BVHNodeFlags::n_positive(i);
    }
}

void BVHTree::update_aabb() {
    m_bounding_box = m_nodes[0].bounding_box;
}

BVHTree::BVHTree(Hittable &hittable) : m_nodes(), m_leaves() {
    hittable.flatten(&m_leaves);
    generate_bvh(0, 0, m_leaves.size(), &m_leaves);
    set_index_buffer_stride(m_stride * 8);
}

void BVHTree::render(SceneRenderer &renderer, BufferChunk &chunk) {
    for (int i = 0; i < m_stride; i++) {
        BVHNode& node = m_nodes[i];

        unsigned int flags = node.flags;
        if (flags & BVHNodeFlags::is_leaf) {
            flags |= renderer.get_hittable_index(*node.leaf);
        }

        chunk.write_index((int) flags);

        Vec3f aabb_boundary = node.get_masked_aabb_vector();
        Vec3f aabb_opposite_boundary = node.get_opposite_masked_aabb_vector();

        chunk.write_vector(aabb_boundary, false);
        chunk.write_vector(aabb_opposite_boundary, true);
    }
}

BVHNode *BVHTree::get_node(int index) {
    if ((int) m_nodes.size() <= index) {
        if (m_nodes.empty()) m_nodes.resize(16, {});
        else m_nodes.resize(m_nodes.size() * 2, {});
    }
    if (index + 1 > m_stride) m_stride = index + 1;
    return &m_nodes[index];
}
