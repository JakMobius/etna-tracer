#version 450 core

layout(std140, push_constant) uniform constants {
    vec4 camera_position;
    vec4 camera_width_vector;
    vec4 camera_height_vector;
    vec4 camera_focus;
    vec4 background;
    ivec2 viewport;
    int entry_index;
    int random_buffer_length;
    int samples;
    int max_reflections;
    uint seed;
} PushConstants;

layout(set = 0, binding = 0) buffer InBuffer {
    ivec4 inBuffer[];
};

layout(set = 0, binding = 1) buffer OutBuffer {
    vec4 outBuffer[];
};

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#include "./structures/hit_record.shader"
#include "./utils/constants.shader"

const int max_stack_size = 24;

HitRecord hit_record;

vec3 ray_source;
vec3 ray_direction;
vec3 inv_ray_direction;

vec3 temp_color;

#include "./utils/random.shader"
#include "./intersections/intersections.shader"
#include "./materials/materials.shader"

void trace_rays() {
    temp_color = vec3(1, 1, 1);
    int reflections = 0;
    vec3 background = PushConstants.background.xyz;

    do {
        hit_record.dist = infinity;
        if(bvh_traverse(PushConstants.entry_index) == -1) return;

        if(isinf(hit_record.dist)) {
            temp_color *= background;
            return;
        }

        ray_source += ray_direction * hit_record.dist;
        if(material_reflect(hit_record.material)) {

            return;
        }
        ray_source += ray_direction * epsilon;
    } while(++reflections < PushConstants.max_reflections);

    if(PushConstants.max_reflections < 0) {
        float color = 1 / hit_record.dist;
        temp_color = vec3(color, color, color);
    } else {
        temp_color = vec3(0, 0, 0);
    }
}

void main() {
    uint index = gl_GlobalInvocationID.x +
                   gl_GlobalInvocationID.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x +
                   gl_GlobalInvocationID.z * gl_NumWorkGroups.x * gl_WorkGroupSize.x * gl_NumWorkGroups.y * gl_WorkGroupSize.y;

    uint pixel_x = index % PushConstants.viewport.x;
    uint pixel_y = index / PushConstants.viewport.x;

    if(pixel_y >= PushConstants.viewport.y) {
        return;
    }

    rand_index = abs(int(hash(hash(uint(pixel_x)) ^ uint(pixel_y)) ^ hash(PushConstants.seed)));

    vec3 result_color = vec3(0, 0, 0);

    for(int i = 0; i < PushConstants.samples; i++) {
        if(i > 0) rand_index = abs(int(hash(rand_index)));

        vec2 position = vec2(float(pixel_x), float(pixel_y)) + vec2(random().xy);
        position /= vec2(PushConstants.viewport);
        position = position * 2 - 1;

        ray_source = PushConstants.camera_position.xyz;
        ray_direction = normalize(PushConstants.camera_focus.xyz + PushConstants.camera_width_vector.xyz * position.x + PushConstants.camera_height_vector.xyz * position.y);

        inv_ray_direction = 1 / ray_direction;
        trace_rays();

        result_color += temp_color;
    }

    outBuffer[index] += vec4(result_color / PushConstants.samples, 1.0);
}