//
// Created by Артем on 01.10.2021.
//

#include "hittable-triangle.hpp"

const int HittableTriangleType = 2;

HittableTriangle::HittableTriangle(const Vec3f &point_a, const Vec3f &point_b, const Vec3f &point_c, Material &material)
        :
        Hittable(), m_point_a(point_a), m_point_b(point_b), m_point_c(point_c), m_material(material) {
    set_index_buffer_stride(28);

    m_normal_a = m_normal_b = m_normal_c = (point_b - point_a).cross(point_c - point_a).normal();
}

void HittableTriangle::render(SceneRenderer &renderer, BufferChunk &chunk) {
    int material_index = renderer.get_material_index(m_material);

    chunk.write_index(HittableTriangleType);
    chunk.write_index(material_index);

    chunk.align();
    chunk.write_vector(m_point_a);
    chunk.write_vector(m_point_b - m_point_a);
    chunk.write_vector(m_point_c - m_point_a);
    chunk.write_vector(m_normal_a);
    chunk.write_vector(m_normal_b);
    chunk.write_vector(m_normal_c);
}

void HittableTriangle::set_normals(const Vec3f &p_normal_a, const Vec3f &p_normal_b, const Vec3f &p_normal_c) {
    m_normal_a = p_normal_a;
    m_normal_b = p_normal_b;
    m_normal_c = p_normal_c;
}

void HittableTriangle::update_aabb() {
    m_bounding_box.lower = m_point_a;
    m_bounding_box.upper = m_point_a;

    for(int i = 0; i < 3; i++) {
        m_bounding_box.lower.set(i, std::min(m_point_a[i], std::min(m_point_b[i], m_point_c[i])));
        m_bounding_box.upper.set(i, std::max(m_point_a[i], std::max(m_point_b[i], m_point_c[i])));
    }

    Vec3f size = m_bounding_box.upper - m_bounding_box.lower;
    Vec3f new_size = size;

    for(int i = 0; i < 3; i++) {
        if(new_size[i] < FLOAT_EPS) new_size.set(i, FLOAT_EPS);
    }

    Vec3f offset = (new_size - size) / 2;
    m_bounding_box.lower -= offset;
    m_bounding_box.upper += offset;
}
