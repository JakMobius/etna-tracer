#pragma once

#include "material.hpp"
#include "../../utils/vec3.hpp"
#include "../buffer-chunk.hpp"

extern const int MaterialMetalType;

class MaterialMetal : public Material {
    Vec3f m_color;
    float m_roughness;
public:

    MaterialMetal(const Vec3f& color, float roughness);

    void render(SceneRenderer&, BufferChunk& chunk) override;

    void set_color(const Vec3f& p_color) { m_color = p_color; }
    const Vec3f& get_color() { return m_color; }
    void set_roughness(float p_roughness) { m_roughness = p_roughness; }
    float get_roughness() { return m_roughness; }
};