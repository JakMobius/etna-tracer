//
// Created by Артем on 02.10.2021.
//

#include "material-lambertian.hpp"

const int MaterialLambertianType = 1;

void MaterialLambertian::render(SceneRenderer &, BufferChunk &chunk) {
    chunk.write_index(MaterialLambertianType);
    chunk.align();
    chunk.write_vector(m_color);
}
