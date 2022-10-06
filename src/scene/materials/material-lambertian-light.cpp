//
// Created by Артем on 02.10.2021.
//

#include "material-lambertian-light.hpp"

const int MaterialLambertianLightType = 2;

void MaterialLambertianLight::render(SceneRenderer &, BufferChunk &chunk) {
    chunk.write_index(MaterialLambertianLightType);
    chunk.align();
    chunk.write_vector(m_color);
}
