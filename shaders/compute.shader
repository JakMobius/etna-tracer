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

    int environment_material;
    int environment_hittable;
} PushConstants;

layout(set = 0, binding = 0) buffer InBuffer {
    ivec4 in_buffer[];
};

layout(set = 0, binding = 1) buffer OutBuffer {
    vec4 out_buffer[];
};

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#include "./utils/constants.shader"
#include "./structures/hit-record.shader"
#include "./structures/ray.shader"
#include "./structures/environment.shader"

#include "./utils/random.shader"
#include "./intersections/intersections.shader"
#include "./materials/materials.shader"
#include "./tracer.shader"

void main() {
    uint index = gl_GlobalInvocationID.x +
                   gl_GlobalInvocationID.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x +
                   gl_GlobalInvocationID.z * gl_NumWorkGroups.x * gl_WorkGroupSize.x * gl_NumWorkGroups.y * gl_WorkGroupSize.y;

    int pixel_x = int(index % PushConstants.viewport.x);
    int pixel_y = int(index / PushConstants.viewport.x);

    if(pixel_y >= PushConstants.viewport.y) {
        return;
    }

    out_buffer[index] += trace_pixel(pixel_x, pixel_y);
}