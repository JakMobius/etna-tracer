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

    void set_color(const Vec3f& color) { m_color = color; }
    const Vec3f& get_color() { return m_color; }
    void set_roughness(float roughness) { m_roughness = roughness; }
    float get_roughness() { return m_roughness; }
};