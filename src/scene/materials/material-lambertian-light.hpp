#pragma once

#include "../../utils/vec3.hpp"
#include "material.hpp"
#include "../buffer-chunk.hpp"

extern const int MaterialLambertianLightType;

class MaterialLambertianLight : public Material {
    Vec3f m_color;
public:

    explicit MaterialLambertianLight(const Vec3f& color): m_color(color) {
        set_index_buffer_stride(8);
    }

    void render(SceneRenderer&, BufferChunk& chunk) override;;

    void set_color(const Vec3f& p_color) { m_color = p_color; }
    const Vec3f& get_color() { return m_color; }
};