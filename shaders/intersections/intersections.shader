
void hittable_hit(int index);

#include "./aabb-intersection.shader"
#include "./sphere-intersection.shader"
#include "./triangle-intersection.shader"
#include "./bvh-intersection.shader"

void hittable_hit(int index) {
    ivec4 hittable_data = in_buffer[index];
    int hittable_type = hittable_data.x & 7;

    switch(hittable_type) {
        case HITTABLE_SPHERE_TYPE:   hittable_sphere_hit(index, hittable_data);   break;
        case HITTABLE_TRIANGLE_TYPE: hittable_triangle_hit(index, hittable_data); break;
        default: return;
    }
}