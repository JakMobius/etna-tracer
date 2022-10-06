#pragma once

#include "../../utils/vec3.hpp"
#include "hittable.hpp"
#include "../materials/material.hpp"
#include "../buffer-chunk.hpp"

extern const int HittableTriangleType;

class HittableTriangle : public Hittable {
    Vec3f m_point_a;
    Vec3f m_point_b;
    Vec3f m_point_c;

    Vec3f m_normal_a;
    Vec3f m_normal_b;
    Vec3f m_normal_c;

    Material& m_material;

public:

    HittableTriangle(const Vec3f& point_a, const Vec3f& point_b, const Vec3f& point_c, Material& material);

    void render(SceneRenderer& renderer, BufferChunk& chunk) override;

    void set_normals(const Vec3f& p_normal_a, const Vec3f& p_normal_b, const Vec3f& p_normal_c);

    void register_materials(SceneRenderer& renderer) override {
        renderer.register_material(m_material);
    }

    void update_aabb() override;

    void set_point_a(const Vec3f& p_point_a) { m_point_a = p_point_a; }
    void set_point_b(const Vec3f& p_point_b) { m_point_b = p_point_b; }
    void set_point_c(const Vec3f& p_point_c) { m_point_c = p_point_c; }

    const Vec3f& get_point_a() { return m_point_a; }
    const Vec3f& get_point_b() { return m_point_b; }
    const Vec3f& get_point_c() { return m_point_c; }
};