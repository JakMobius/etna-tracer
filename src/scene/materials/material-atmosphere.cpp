
#include "material-atmosphere.hpp"

const int MaterialAtmosphereType = 4;

MaterialAtmosphere::MaterialAtmosphere() {
    set_index_buffer_stride(16);
}

void MaterialAtmosphere::render(SceneRenderer &, BufferChunk &chunk) {
    // First vec4
    chunk.write_index(MaterialAtmosphereType);
    chunk.write_vector(m_sun_position, false);

    // Second vec4
    chunk.write_vector(m_scattering_coefficients, false);
    chunk.write_float(m_intensity);

    // Third vec4
    chunk.write_vector(m_atmosphere_center, false);
    chunk.write_float(m_atmosphere_radius);

    // Fourth vec4
    chunk.write_float(m_density_falloff);
}

void MaterialAtmosphere::set_scattering_coefficients_from_wavelengths(float wavelength_r, float wavelength_g,
                                                                      float wavelength_b, float strength) {
    m_scattering_coefficients = Vec3f {
            static_cast<float>(pow(1 / wavelength_r, 4)),
            static_cast<float>(pow(1 / wavelength_g, 4)),
            static_cast<float>(pow(1 / wavelength_b, 4))
    } *= strength;
}
