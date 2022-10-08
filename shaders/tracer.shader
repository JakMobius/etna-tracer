
void trace_rays() {
    int reflections = 0;
    vec3 background = PushConstants.background.xyz;

    do {
        hit_record.dist = infinity;
        if(bvh_traverse(PushConstants.entry_index) == -1) return;

        if(environment_scatter()) {
            continue;
        }

        if(isinf(hit_record.dist)) {
            ray.light_received += background * ray.color_mask;
            return;
        }

        ray.source += ray.direction * hit_record.dist;
        if(material_reflect()) {
            return;
        }

        // Multiply by the ray.source length to make epsilon relative to the ray length.
        // The farther the ray, the more epsilon we need to avoid self-intersection.
        ray.source += ray.direction * epsilon * length(ray.source);
    } while(++reflections < PushConstants.max_reflections);

    if(PushConstants.max_reflections < 0) {
        float color = 1 / hit_record.dist;
        ray.light_received = vec3(color, color, color);
    } else {
        ray.light_received = vec3(0, 0, 0);
    }
}

vec4 trace_pixel(int pixel_x, int pixel_y) {
    update_rand_index(pixel_x, pixel_y, PushConstants.seed);

    vec3 result_color = vec3(0, 0, 0);

    environment_toggle(-1, 0);

    for(int i = 0; i < PushConstants.samples; i++) {
        vec2 position = vec2(float(pixel_x), float(pixel_y)) + random_vec2();
        position /= vec2(PushConstants.viewport);
        position = position * 2 - 1;

        env_ptr = 0;
        if(PushConstants.environment_hittable >= 0) {
            environment_toggle(PushConstants.environment_hittable, PushConstants.environment_material);
        }

        ray.source = PushConstants.camera_position.xyz;
        ray.color_mask = vec3(1, 1, 1);
        ray.light_received = vec3(0, 0, 0);

        set_ray_direction(normalize(PushConstants.camera_focus.xyz + PushConstants.camera_width_vector.xyz * position.x + PushConstants.camera_height_vector.xyz * position.y));
        trace_rays();

        if(isinf(hit_record.dist) && env_stack[env_ptr][0] != -1) {
            // As the floating point precision is limited, sometimes required invariants are not met.
            // In example, the ray may hit the same spot twice if the epsilon is too small.
            // In this case, the environment stack will not be correct anymore, as it will
            // think that the ray is still inside the environment, while it is actually
            // flying all the way to the background. This, in order, might cause some NaNs
            // to appear, so this ray should be discarded. This should be a rare case to happen,
            // so ignore the fact that this sample is lost.
            continue;
        }

        result_color += ray.light_received;
    }

    return vec4(result_color / PushConstants.samples, 1.0);
}