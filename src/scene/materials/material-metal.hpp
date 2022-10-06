#pragma once

#include "material.hpp"
#include "../../utils/vec3.hpp"
#include "../buffer-chunk.hpp"

extern const int MaterialMetalType;

class MaterialMetal : public Material {
    Vec3f color;
    float roughness;
public:

    MaterialMetal(const Vec3f& color, float roughness): color(color), roughness(roughness) {
        set_index_buffer_stride(8);
    }

    virtual void render(SceneRenderer&, BufferChunk& chunk) override {
        chunk.write_index(MaterialMetalType);
        chunk.align();
        chunk.write_vector(color, false);
        chunk.write_float(roughness);
    };

    void set_color(const Vec3f& p_color) { color = p_color; }
    const Vec3f& get_color() { return color; }
    void set_roughness(float p_roughness) { roughness = p_roughness; }
    float get_roughness() { return roughness; }
};