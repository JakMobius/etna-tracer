#pragma once

#include "hittable.hpp"
#include "../../utils/vec3.hpp"
#include "../materials/material.hpp"
#include "../buffer-chunk.hpp"

const int HittableSphereType = 1;

class HittableSphere : public Hittable {
    Vec3f position;
    float radius;
    Material& material;
public:

    HittableSphere(const Vec3f& position, float radius, Material& material): Hittable(), position(position), radius(radius), material(material) {
        set_index_buffer_stride(8);
    }

    void render(SceneRenderer& renderer, BufferChunk& chunk) override {
        int material_index = renderer.get_material_index(material);

        chunk.write_index(HittableSphereType);
        chunk.write_index(material_index);

        chunk.align();
        chunk.write_vector(position, false);
        chunk.write_float(radius);
    }

    void register_materials(SceneRenderer& renderer) override {
        renderer.register_material(material);
    }

    void set_position(const Vec3f& p_position) { position = p_position; }
    const Vec3f& get_position() const { return position; }
    void set_radius(float p_radius) { radius = p_radius; }
    float get_radius() const { return radius; }

    void update_aabb() override {
        Vec3f radius_vector { radius, radius, radius };

        bounding_box.lower = position;
        bounding_box.upper = position;

        bounding_box.lower -= radius_vector;
        bounding_box.upper += radius_vector;
    }
};