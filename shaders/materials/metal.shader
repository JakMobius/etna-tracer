
const int MATERIAL_METAL = 0;

void material_metal_reflect(int index, ivec4 data) {
    vec4 material_color = uintBitsToFloat(in_buffer[index + 1]);
    float fuzziness = material_color.a;

    ray.color_mask *= material_color.rgb;
    vec3 direction = reflect(ray.direction, hit_record.normal);

    vec3 random_vec = random_vec3();

    direction += normalize(fuzziness * random_vec + ray.direction);
    float projection = dot(direction, hit_record.surface_normal);

    if(projection < 0) {
        direction -= hit_record.surface_normal * projection * 2;
    }

    set_ray_direction(normalize(ray.direction));
}