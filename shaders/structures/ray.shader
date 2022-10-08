
struct Ray {
    vec3 direction;
    vec3 inv_direction;
    vec3 source;
    vec3 light_received;
    vec3 color_mask;
};

Ray ray;

void set_ray_direction(vec3 dir) {
    ray.direction = dir;
    ray.inv_direction = 1.0 / dir;
}