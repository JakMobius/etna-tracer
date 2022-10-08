
const int MATERIAL_LAMBERTIAN = 1;

void material_lambertian_reflect(int index, ivec4 data) {
    vec3 material_color = uintBitsToFloat(data.yzw);
    ray.color_mask *= material_color;

    vec3 direction = normalize(hit_record.normal + random_unit_vec3());

    float projection = dot(direction, hit_record.surface_normal);

    if(projection < 0) {
        direction -= hit_record.surface_normal * projection * 2;
    }

    set_ray_direction(direction);
}