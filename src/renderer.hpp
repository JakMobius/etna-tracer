#pragma once

#include <etna/init.hpp>
#include <random>
#include <utility>
#include "compute-program.hpp"
#include "scene/scene.hpp"
#include "scene/materials/material-lambertian-light.hpp"
#include "scene/materials/material-lambertian.hpp"
#include "scene/hittables/hittable.hpp"
#include "scene/hittables/hittable-list.hpp"
#include "utils/camera.hpp"
#include "scene/hittables/hittable-sphere.hpp"
#include "scene/build-cube.hpp"
#include "utils/vec2.hpp"

#pragma pack(push, 1)
struct RendererPushConstants {
    float camera_position[4];
    float camera_width_vector[4];
    float camera_height_vector[4];
    float camera_focus[4];
    float background[4];
    int viewport[2];
    int entry_index;
    int random_buffer_length;
    int samples;
    int max_reflections;
    uint seed;
    int environment_material;
    int environment_hittable;
};
#pragma pack(pop)

class App;

class Renderer {

    App& m_app;
    std::unique_ptr<ComputeProgram> m_compute_program;
    RendererPushConstants m_push_constants {};
    std::unique_ptr<ComputeAppBuffer> m_scene_compute_buffer;
    std::unique_ptr<ComputeAppBuffer> m_compute_framebuffer;
    std::vector<char> m_framebuffer;
    std::random_device m_random_device;
    std::mt19937 m_random_generator;
    VK::ShaderModule m_shader_module;

    int m_accumulated_frames = 0;
    int m_max_reflections = 10;
    int m_samples = 1;

public:

    Renderer(App& app, const VK::PhysicalDevice& device);

    int initialize();

    int get_accumulated_frames() const {
        return m_accumulated_frames;
    }

    int get_samples() {
        return m_samples;
    }

    void set_screen_size(const Vec2<int>& size) {
        m_push_constants.viewport[0] = size[0];
        m_push_constants.viewport[1] = size[1];
    }

    void set_environment_hittable(int index) {
        m_push_constants.environment_hittable = index;
    }

    void set_environment_material(int index) {
        m_push_constants.environment_material = index;
    }

    void set_entry_index(int index) {
        m_push_constants.entry_index = index;
    }

    int get_max_reflections() const {
        return m_max_reflections;
    }

    void set_samples(int samples) {
        m_samples = samples;
    }

    void set_max_reflections(int max_reflections) {
        m_max_reflections = max_reflections;
    }

    void set_background(const Vec3f& color) {
        m_push_constants.background[0] = color[0];
        m_push_constants.background[1] = color[1];
        m_push_constants.background[2] = color[2];
    }

    ComputeProgram& get_program() {
        return *m_compute_program;
    }

    void trigger_rendering();
    void clear_framebuffer();

    void download_framebuffer() {
        m_compute_framebuffer->set_should_download(true);
        m_compute_program->download_buffers();
    }

    void set_input_buffer_data(const std::vector<int>& data) {
        std::span<char> input_data = {(char *) &data[0], data.size() * sizeof(data[0])};
        m_scene_compute_buffer->set_host_buffer(input_data);
        m_scene_compute_buffer->set_should_upload(true);
    }

    std::span<char> &get_framebuffer();

private:

    void setup_push_constants();
};