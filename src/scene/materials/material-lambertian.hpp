#pragma once

#include "material.hpp"
#include "../../utils/vec3.hpp"
#include "../buffer-chunk.hpp"

extern const int MaterialLambertianType;

class MaterialLambertian : public Material {
    Vec3f m_color;
public:

    explicit MaterialLambertian(const Vec3f& color): m_color(color) {
        set_index_buffer_stride(4);
    }

    void render(SceneRenderer&, BufferChunk& chunk) override;;

    void set_color(const Vec3f& color) { m_color = color; }
    const Vec3f& get_color() { return m_color; }
};