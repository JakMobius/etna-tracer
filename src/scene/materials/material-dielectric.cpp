//
// Created by Артем on 07.10.2021.
//

#include "material-dielectric.hpp"

const int MaterialDielectricType = 3;

MaterialDielectric::MaterialDielectric(const Vec3f &color, float refr_coef, float roughness, float fuzziness) :
        m_color(color),
        m_roughness(roughness),
        m_refr_coef(refr_coef),
        m_fuzziness(fuzziness) {
    set_index_buffer_stride(12);
}

void MaterialDielectric::render(SceneRenderer &, BufferChunk &chunk) {

    chunk.write_index(MaterialDielectricType);

    chunk.align();
    chunk.write_vector(m_color);
    chunk.write_float(m_roughness);
    chunk.write_float(m_refr_coef);
    chunk.write_float(m_fuzziness);
}
