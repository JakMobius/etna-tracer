//
// Created by Артем on 01.10.2021.
//

#include "hittable-sphere.hpp"

HittableSphere::HittableSphere(const Vec3f &position, float radius, Material &material) : Hittable(), m_position(position), m_radius(radius), m_material(material) {
    set_index_buffer_stride(8);
}

void HittableSphere::render(SceneRenderer &renderer, BufferChunk &chunk) {
    int material_index = renderer.get_material_index(m_material);

    chunk.write_index(HittableSphereType);
    chunk.write_index(material_index);

    chunk.align();
    chunk.write_vector(m_position, false);
    chunk.write_float(m_radius);
}

void HittableSphere::update_aabb() {
    Vec3f radius_vector {m_radius, m_radius, m_radius };

    m_bounding_box.lower = m_position;
    m_bounding_box.upper = m_position;

    m_bounding_box.lower -= radius_vector;
    m_bounding_box.upper += radius_vector;
}
