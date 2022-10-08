
const int MATERIAL_DIELECTRIC = 3;

float fresnel(vec3 I, vec3 N, float etai, float etat) {
    float cosi = dot(I, N);

    // Compute sint using Snell's law
    float sint = etai / etat * sqrt(1 - cosi * cosi);

    // Total internal reflection
    if (sint >= 1) return 1.0;

    float cost = sqrt(max(0.f, 1 - sint * sint));
    cosi = abs(cosi);
    float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    return (Rs * Rs + Rp * Rp) / 2;
}

void material_dielectric_reflect(int index, ivec4 data) {
    vec3 material_color = uintBitsToFloat(in_buffer[index + 1].xyz);
    vec3 material_data = uintBitsToFloat(in_buffer[index + 2].xyz);

    float refr_coef = material_data.g;
    float fuzziness = material_data.b;

    ray.color_mask *= material_color.rgb;

    vec3 random_vec = random_vec3();
    bool should_reflect = true;

    if(hit_record.front_hit) {
        should_reflect = fresnel(ray.direction, hit_record.normal, 1, refr_coef) >= random_vec.x;
        refr_coef = 1 / refr_coef;
    } else {
        should_reflect = fresnel(ray.direction, -hit_record.normal, refr_coef, 1) >= random_vec.x;
    }

    vec3 scatter_direction;

    if (should_reflect) {
        scatter_direction = reflect(ray.direction, hit_record.normal);
    } else {
        scatter_direction = refract(normalize(ray.direction), normalize(hit_record.normal), refr_coef);
    }

    if(fuzziness > 0) {
        vec3 fuzz_scatter = random_unit_vec3() * fuzziness;
        scatter_direction += fuzz_scatter;
    }

    set_ray_direction(scatter_direction);
}