//
// Created by Артем on 02.10.2021.
//

#include "material-metal.hpp"

const int MaterialMetalType = 0;

MaterialMetal::MaterialMetal(const Vec3f &color, float roughness) : m_color(color), m_roughness(roughness) {
    set_index_buffer_stride(8);
}

void MaterialMetal::render(SceneRenderer &, BufferChunk &chunk) {
    chunk.write_index(MaterialMetalType);
    chunk.align();
    chunk.write_vector(m_color, false);
    chunk.write_float(m_roughness);
}
