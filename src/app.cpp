
#include "app.hpp"
#include <sstream>

int App::main() {
    if (!Etna::initialize()) {
        std::cerr << "Could not find Vulkan library\n";
        return EXIT_FAILURE;
    }

    VK::InstanceFactory factory{};

    factory.set_app_name("Etna Compute Example");
    factory.set_app_version({1, 0, 0});
    factory.get_enabled_extension_names().push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    factory.get_enabled_layer_names().push_back("VK_LAYER_KHRONOS_validation");

    m_vk_instance = factory.create();

    VK::DebugCallbackHandler m_debug_callback_handler{};

    if (!m_debug_callback_handler.listen(m_vk_instance.get_handle())) {
        throw std::runtime_error("debug callback handler failed to initialize");
    }

    setup_scene();

    for (auto &device: m_vk_instance.get_physical_devices()) {
        // Make sure it is a discrete GPU
        if (device.get_physical_properties()->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            continue;
        }

        auto renderer = std::make_unique<Renderer>(*this, device);
        renderer->initialize();
        renderer->set_screen_size({m_screen_width, m_screen_height});
        m_renderers.push_back(std::move(renderer));
    }

    int max_frames = 100;

    for(int i = 0; ; i++) {
        float t = static_cast<float>(i) / static_cast<float>(max_frames);

        tick_scene(t);

        if(m_rendering) {
            wait_for_threads();

            std::stringstream ss;
            ss << "output/frame_" << (i - 1) << ".ppm";
            write_output_to_file(ss.str());
        }

        if(i >= max_frames) {
            break;
        }

        for(auto& renderer: m_renderers) {
            renderer->set_input_buffer_data(m_scene_buffer.get_index_buffer());
            renderer->set_environment_hittable(m_atmosphere_sphere_index);
            renderer->set_environment_material(m_atmosphere_material_index);
            renderer->set_entry_index(m_scene_buffer.get_entry_hittable_index());
        }

        render();
    }

    return 0;
}

void App::render() {
    m_render_start_time = std::chrono::high_resolution_clock::now();

    for (auto &renderer: m_renderers) {
        m_threads.emplace_back([&renderer, this] {

            renderer->clear_framebuffer();
            auto start = std::chrono::high_resolution_clock::now();

            while (!m_stop) {
                renderer->trigger_rendering();
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start).count());
            auto rays = static_cast<float>(renderer->get_accumulated_frames()) *
                        static_cast<float>(renderer->get_samples()) *
                        static_cast<float>(m_screen_width * m_screen_height);

            auto rays_per_second = static_cast<unsigned long long>(rays / duration * 1000);

            std::cout << "Device "
                      << renderer->get_program().get_physical_device().get_physical_properties()->deviceName
                      << " rendered " << renderer->get_accumulated_frames()
                      << " frames, "
                      << rays_per_second << " rays per second\n";
            renderer->download_framebuffer();
        });
    }

    m_rendering = true;
}

void App::wait_for_threads() {
    if(!m_rendering) {
        return;
    }

    std::this_thread::sleep_until(m_render_start_time + std::chrono::seconds(m_render_time));

    m_stop = true;
    for (auto &thread: m_threads) {
        thread.join();
    }
    m_threads.clear();
    m_stop = false;
    m_rendering = false;
}

void App::write_output_to_file(const std::string& file_name) {

    std::ofstream file(file_name);
    file << "P6\n" << m_screen_width << " " << m_screen_height << "\n" << "255\n";

    std::vector<float> sum_pixels;
    int pixel_array_size = m_screen_width * m_screen_height * 4;
    sum_pixels.resize(pixel_array_size);

    float total_frames = 0;

    for (auto &renderer: m_renderers) {
        auto *renderer_framebuffer = reinterpret_cast<float *>(renderer->get_framebuffer().data());
        auto renderer_frames = static_cast<float>(renderer->get_accumulated_frames());
        total_frames += renderer_frames;

        for (int i = 0; i < pixel_array_size; i++) {
            sum_pixels[i] += renderer_framebuffer[i];
        }
    }

    for (int i = 0; i < pixel_array_size; i++) {
        sum_pixels[i] = pow(sum_pixels[i] / total_frames, m_gamma);
    }

    for (int y = 0; y < m_screen_height; y++) {
        for (int x = 0; x < m_screen_width; x++) {
            int index = (y * m_screen_width + x) * 4;
            float *pixel = &sum_pixels[index];

            for (int ch = 0; ch < 3; ch++) {
                float data = pixel[ch];
                if (data > 1) {
                    data = 1;
                }
                if (data < 0) {
                    data = 0;
                }
                file << static_cast<char>(data * 255);
            }
        }
    }
}

void App::setup_scene() {
    float atmosphere_scale = 0.016;

    auto &sun_material = m_scene.create<MaterialLambertianLight>(Vec3f{1, 1, 1} * 2);
    m_sun_sphere = &m_scene.create<HittableSphere>(Vec3f {}, 2400000, sun_material);

    auto &planet_material = m_scene.create<MaterialLambertian>(Vec3f{0.7, 1.0, 0.9});
    auto &planet_sphere = m_scene.create<HittableSphere>(Vec3f{0, 0, 1 * 1000 * 1000}, 1 * 1000 * 1000,
                                                         planet_material);

    m_atmosphere = &m_scene.create<MaterialAtmosphere>();
    m_atmosphere_sphere = &m_scene.create<HittableSphere>(planet_sphere.get_position(),
                                                             planet_sphere.get_radius() * (1 + atmosphere_scale),
                                                             *m_atmosphere);

    m_atmosphere->set_sun_position(m_sun_sphere->get_position());
    m_atmosphere->set_scattering_coefficients_from_wavelengths(0.700, 0.530, 0.460, 800);
    m_atmosphere->set_atmosphere_center(m_atmosphere_sphere->get_position());
    m_atmosphere->set_atmosphere_radius(m_atmosphere_sphere->get_radius());
    m_atmosphere->set_density_falloff(2);
    m_atmosphere->set_intensity(2);

    m_scene.get_root_hittable().add_child(*m_sun_sphere);
    m_scene.get_root_hittable().add_child(planet_sphere);
    m_scene.get_root_hittable().add_child(*m_atmosphere_sphere);

    if (m_camera_in) {
        m_camera.set_position({0, 0, -2.9});
        m_camera.set_focus_distance(3);
        m_camera.matrix = Matrix4f::rotation_z_matrix(0 * M_PI);
        m_camera.set_moved();
    } else {
        m_camera.set_position({-18 * 1000 * 1000, 0, -2 * 1000 * 1000});
        m_camera.set_focus_distance(2);
        m_camera.matrix = m_camera.matrix * Matrix4f::rotation_y_matrix(0.2);
        m_camera.set_moved();
    }
}

void App::tick_scene(float t) {

    float angle_from = -M_PI * 0.14;
    float angle_to = M_PI * 0.14;
    t = angle_to * t + angle_from * (1 - t);

    float x = cos(t) * 50;
    float y = sin(t) * 30;
    float z = sin(t) * 50;

    m_sun_sphere->set_position(Vec3f{x, y, z} * 1000000);
    m_atmosphere->set_sun_position(m_sun_sphere->get_position());

    m_scene_buffer.get_index_buffer().clear();
    m_renderer.render(m_scene_buffer, m_scene);

    if(m_camera_in) {
        m_atmosphere_material_index = m_renderer.get_material_index(*m_atmosphere);
        m_atmosphere_sphere_index = m_renderer.get_hittable_index(*m_atmosphere_sphere);
    } else {
        m_atmosphere_material_index = -1;
        m_atmosphere_sphere_index = -1;
    }
}