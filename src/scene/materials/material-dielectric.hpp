#pragma once

#include "material.hpp"
#include "../buffer-chunk.hpp"

extern const int MaterialDielectricType;

class MaterialDielectric : public Material {
    Vec3f m_color;
    float m_roughness;
    float m_refr_coef;
    float m_fuzziness;
public:

    explicit MaterialDielectric(const Vec3f& color, float refr_coef = 1, float roughness = -1, float fuzziness = 0);

    virtual void render(SceneRenderer&, BufferChunk& chunk) override;;

    void set_color(const Vec3f& p_color) { m_color = p_color; }
    const Vec3f& get_color() { return m_color; }

    void set_roughness(float p_roughness) { m_roughness = p_roughness; }
    float get_roughness() { return m_roughness; }
    void set_refr_coef(float p_refr_coef) { m_refr_coef = p_refr_coef; }
    float get_refr_coef() { return m_refr_coef; }
    void set_fuzziness(float p_fuzziness) { m_fuzziness = p_fuzziness; }
    float get_fuzziness() { return m_fuzziness; }
};