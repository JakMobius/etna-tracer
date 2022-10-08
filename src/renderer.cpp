
#include "renderer.hpp"
#include "app.hpp"

Renderer::Renderer(App &app, const VK::PhysicalDevice &device) : m_app(app),
                                                                 m_random_generator(m_random_device()) {
    m_compute_program = std::make_unique<ComputeProgram>(app.get_vk_instance(), device);
}

int Renderer::initialize() {
    m_compute_program->initialize();

    m_shader_module = VK::ShaderModule::from_file(&m_compute_program->get_device(), "../shaders/compute.spv");

    m_compute_program->set_shader(m_shader_module);

    setup_push_constants();

    m_framebuffer.resize((m_app.get_screen_width() * m_app.get_screen_height() * 4 * sizeof(float)));

    m_scene_compute_buffer = std::make_unique<ComputeAppBuffer>(0);
    m_compute_framebuffer = std::make_unique<ComputeAppBuffer>(1);

    m_scene_compute_buffer->set_should_upload(true);

    m_compute_program->add_buffer(*m_scene_compute_buffer);
    m_compute_program->add_buffer(*m_compute_framebuffer);

    m_compute_framebuffer->set_host_buffer(m_framebuffer);

    m_compute_program->get_push_constants().data = {(char *) &m_push_constants, sizeof(RendererPushConstants)};

    return 0;
}

void Renderer::clear_framebuffer() {
    m_accumulated_frames = 0;
    std::fill(m_framebuffer.begin(), m_framebuffer.end(), 0);
    m_compute_framebuffer->set_should_upload(true);
}

void Renderer::trigger_rendering() {
    int x = 32;
    int z = 4;

    int screen_width = m_app.get_screen_width();
    int screen_height = m_app.get_screen_height();

    m_push_constants.seed = m_random_generator();
    m_compute_program->compute(x, screen_width * screen_height / (x * z), z);
    m_accumulated_frames++;
}

std::span<char> &Renderer::get_framebuffer() {
    return m_compute_framebuffer->get_host_buffer();
}

void Renderer::setup_push_constants() {
    m_push_constants.viewport[0] = 800;
    m_push_constants.viewport[1] = 800;

    auto camera = m_app.get_camera();

    m_push_constants.camera_position[0] = camera.get_position()[0];
    m_push_constants.camera_position[1] = camera.get_position()[1];
    m_push_constants.camera_position[2] = camera.get_position()[2];

    m_push_constants.camera_focus[0] = camera.get_calculated_focus_vector()[0];
    m_push_constants.camera_focus[1] = camera.get_calculated_focus_vector()[1];
    m_push_constants.camera_focus[2] = camera.get_calculated_focus_vector()[2];

    m_push_constants.camera_height_vector[0] = camera.get_calculated_height_vector()[0];
    m_push_constants.camera_height_vector[1] = camera.get_calculated_height_vector()[1];
    m_push_constants.camera_height_vector[2] = camera.get_calculated_height_vector()[2];

    m_push_constants.camera_width_vector[0] = camera.get_calculated_width_vector()[0];
    m_push_constants.camera_width_vector[1] = camera.get_calculated_width_vector()[1];
    m_push_constants.camera_width_vector[2] = camera.get_calculated_width_vector()[2];

    m_push_constants.max_reflections = m_max_reflections;
    m_push_constants.samples = m_samples;
}
