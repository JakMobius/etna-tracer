
#include "app.hpp"

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

    std::cout << &m_scene.get_root_hittable() << "\n";

    setup_scene();

    for (auto &device: m_vk_instance.get_physical_devices()) {
        auto renderer = std::make_unique<Renderer>(*this, device);
        renderer->initialize();
        renderer->set_background({0.049/2, 0.041/2, 0.159/2});
        m_renderers.push_back(std::move(renderer));
    }

    render();

    std::this_thread::sleep_for(std::chrono::seconds(m_render_time));

    m_stop = true;

    wait_for_threads();
    write_output_to_file();

    return 0;
}

void App::render() {
    for (auto &renderer: m_renderers) {
        m_threads.emplace_back([&renderer, this] {

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
}

void App::wait_for_threads() {
    for (auto &thread: m_threads) {
        thread.join();
    }
}

void App::write_output_to_file() {
    // write m_result_buffer to image.ppm

    std::ofstream file("image.ppm");
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

    for(int i = 0; i < pixel_array_size; i++) {
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
