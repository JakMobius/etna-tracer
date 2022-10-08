
const int MATERIAL_ATMOSPHERE = 4;

vec2 sphere_distance(vec3 sphere_position, float sphere_radius, vec3 ray_start, vec3 ray_direction) {
    vec3 offset = ray_start - sphere_position;
    float a = 1;
    float b = 2 * dot(ray_direction, offset);
    float c = dot(offset, offset) - sphere_radius * sphere_radius;
    float discriminant = b * b - 4 * a * c;

    if(discriminant > 0) {
        float droot = sqrt(discriminant);
        float near_t = max(0, (-b - droot) / (2 * a));
        float far_t = (-b + droot) / (2 * a);

        if(far_t < 0) {
            return vec2(infinity, 0);
        }

        return vec2(near_t, far_t - near_t);
    }

    return vec2(infinity, 0);
}

float atmosphere_density_at_height(float height, float atmosphere_height, float density_falloff) {
    if(height > atmosphere_height) {
        return 0;
    }
    float normalized_height = height / atmosphere_height;
    return exp(-normalized_height * density_falloff) * (1 - normalized_height);
}

float optical_depth(vec3 ray_start, vec3 ray_direction, float ray_length, vec3 atmosphere_center, float atmosphere_height, float density_falloff) {
    float optical_depth = 0;
    int step_count = 10;
    float step_size = ray_length / float(step_count - 1);

    vec3 pos = ray_start;

    for(int i = 0; i < step_count; i++) {
        float height = length(pos - atmosphere_center);
        float density = atmosphere_density_at_height(height, atmosphere_height, density_falloff);
        optical_depth += density * step_size / atmosphere_height;
        pos += ray_direction * step_size;
    }

    return optical_depth;
}

void material_atmosphere_scatter(int index, ivec4 data) {
    float dist = hit_record.dist;

    if(isinf(dist)) {
        return;
    }

    vec3 light_received = vec3(0.0);

    int iterations = 10;
    float step_size = dist / float(iterations);

    vec3 pos = ray.source;

    vec3 sun_pos = uintBitsToFloat(data.yzw);
    vec4 data1 = uintBitsToFloat(in_buffer[index + 1]);
    vec4 data2 = uintBitsToFloat(in_buffer[index + 2]);
    vec4 data3 = uintBitsToFloat(in_buffer[index + 3]);

    vec3 scattering_coefficients = vec3(data1.xyz);
    float intensity = data1.w;

    vec3 atmosphere_center = vec3(data2.xyz);
    float atmosphere_radius = data2.w;

    float density_falloff = data3.x;

    for(int i = 0; i < iterations; i++) {
        vec3 sun_dir = normalize(sun_pos - pos);
        float height_above_atmosphere_center = length(pos - atmosphere_center);

        float sun_atmosphere_length = sphere_distance(atmosphere_center, atmosphere_radius, pos, sun_dir).y;
        float sun_optical_depth = optical_depth(pos, sun_dir, sun_atmosphere_length, atmosphere_center, atmosphere_radius, density_falloff);
        float ray_optical_depth = optical_depth(pos, -ray.direction, step_size * float(i), atmosphere_center, atmosphere_radius, density_falloff);

        float angle_cos = dot(ray.direction, sun_dir);
        float rayleigh_coef = (1 + angle_cos * angle_cos) / 2;

        vec3 transmittance = exp((-sun_optical_depth - ray_optical_depth) * scattering_coefficients) * rayleigh_coef;

        float local_atmosphere_density = atmosphere_density_at_height(height_above_atmosphere_center, atmosphere_radius, density_falloff);

        light_received += local_atmosphere_density * transmittance * step_size * scattering_coefficients;

        pos += ray.direction * step_size;
    }

    ray.light_received += light_received * ray.color_mask / atmosphere_radius * intensity;

    float ray_optical_depth = optical_depth(pos, -ray.direction, dist, atmosphere_center, atmosphere_radius, density_falloff);

    ray.color_mask *= exp(-ray_optical_depth * scattering_coefficients);
}