#pragma once

#include "hittable.hpp"
#include "../../utils/vec3.hpp"
#include "../materials/material.hpp"
#include "../buffer-chunk.hpp"

const int HittableSphereType = 1;

class HittableSphere : public Hittable {
    Vec3f m_position;
    float m_radius;
    Material& m_material;
public:

    HittableSphere(const Vec3f& position, float radius, Material& material);

    void render(SceneRenderer& renderer, BufferChunk& chunk) override;

    void register_materials(SceneRenderer& renderer) override {
        renderer.register_material(m_material);
    }

    void set_position(const Vec3f& p_position) { m_position = p_position; }
    const Vec3f& get_position() const { return m_position; }
    void set_radius(float p_radius) { m_radius = p_radius; }
    float get_radius() const { return m_radius; }

    void update_aabb() override;
};