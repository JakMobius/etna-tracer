
const int MATERIAL_LAMBERTIAN_LIGHT = 2;

void material_light_reflect(int index, ivec4 data) {
    vec3 material_color = uintBitsToFloat(data.yzw);
    ray.light_received += material_color * ray.color_mask;
}