#pragma once

#include "material-atmosphere.hpp"
#include "material.hpp"
#include "../scene-renderer.hpp"

class MaterialAtmosphere : public Material {
    Vec3f m_sun_position;
    Vec3f m_scattering_coefficients;
    Vec3f m_atmosphere_center;
    float m_atmosphere_radius;
    float m_intensity;
    float m_density_falloff;
public:
    MaterialAtmosphere();

    void set_sun_position(const Vec3f& sun_position) { m_sun_position = sun_position; }

    void render(SceneRenderer &, BufferChunk &chunk) override;

    void set_scattering_coefficients_from_wavelengths(float wavelength_r, float wavelength_g, float wavelength_b, float strength);

    void set_atmosphere_center(const Vec3f& center) {
        m_atmosphere_center = center;
    }

    void set_atmosphere_radius(float radius) {
        m_atmosphere_radius = radius;
    }

    void set_intensity(float intensity) {
        m_intensity = intensity;
    }

    void set_density_falloff(float density_falloff) {
        m_density_falloff = density_falloff;
    }
};