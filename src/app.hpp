
#include <etna/init.hpp>
#include <etna/vk-wrappers/device/vk-physical-device.hpp>
#include <iostream>
#include <thread>
#include <etna/vk-wrappers/instance/vk-instance-factory.hpp>
#include <etna/vk-wrappers/vk-debug-callback-handler.hpp>
#include "renderer.hpp"

class App {
    VK::Instance m_vk_instance;
    Scene m_scene;
    Camera m_camera;
    SceneRenderer m_renderer;
    SceneBuffer m_scene_buffer;
    std::vector<std::unique_ptr<Renderer>> m_renderers;
    int m_screen_width = 800;
    int m_screen_height = 800;

    std::atomic<bool> m_stop { false };
    std::vector<std::thread> m_threads;

public:
    int main() {
        if(!Etna::initialize()) {
            std::cerr << "Could not find Vulkan library\n";
            return EXIT_FAILURE;
        }

        VK::InstanceFactory factory {};

        factory.set_app_name("Etna Compute Example");
        factory.set_app_version({1, 0, 0});
        factory.get_enabled_extension_names().push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        factory.get_enabled_layer_names().push_back("VK_LAYER_KHRONOS_validation");

        m_vk_instance = factory.create();

        VK::DebugCallbackHandler m_debug_callback_handler {};

        if(!m_debug_callback_handler.listen(m_vk_instance.get_handle())) {
            throw std::runtime_error("debug callback handler failed to initialize");
        }

        std::cout << &m_scene.get_root_hittable() << "\n";

        setup_scene();

        for(auto& device : m_vk_instance.get_physical_devices()) {
//            std::cout << "Device: " << device.get_physical_properties()->deviceName << "\n";

            auto renderer = std::make_unique<Renderer>(*this, device);
            renderer->initialize();
            m_renderers.push_back(std::move(renderer));
        }

        render();

        // Wait 10s;
        std::this_thread::sleep_for(std::chrono::seconds(10));

        m_stop = true;

        wait_for_threads();
        write_output_to_file();

        return 0;
    }

    void render() {
        for(auto& renderer : m_renderers) {
            m_threads.emplace_back([&renderer, this] {

                auto start = std::chrono::high_resolution_clock::now();

                while(!m_stop) {
                    renderer->trigger_rendering();
                }

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
                auto rays = static_cast<float>(renderer->get_accumulated_frames() * renderer->get_samples() * m_screen_width * m_screen_height);

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

    void wait_for_threads() {
        for(auto& thread : m_threads) {
            thread.join();
        }
    }

    int get_screen_width() const {
        return m_screen_width;
    }

    int get_screen_height() const {
        return m_screen_height;
    }

    void write_output_to_file() {
        // write m_result_buffer to image.ppm

        std::ofstream file("image.ppm");
        file << "P3\n";
        file << m_screen_width << " " << m_screen_height << "\n";
        file << "255\n";

        std::vector<float> sum_pixels;
        int pixel_array_size = m_screen_width * m_screen_height * 4;
        sum_pixels.resize(pixel_array_size);

        float total_frames = 0;

        for(auto& renderer : m_renderers) {
            auto* renderer_framebuffer = reinterpret_cast<float*>(renderer->get_framebuffer().data());
            auto renderer_frames = static_cast<float>(renderer->get_accumulated_frames());
            total_frames += renderer_frames;

            for(int i = 0; i < pixel_array_size; i++) {
                sum_pixels[i] += renderer_framebuffer[i];
            }
        }

        for (int y = 0; y < m_screen_height; y++) {
            for (int x = 0; x < m_screen_width; x++) {
                int index = (y * m_screen_width + x) * 4;
                float* pixel = &sum_pixels[index];

                for (int ch = 0; ch < 3; ch++) {
                    float data = pixel[ch] / total_frames;
                    if (data > 1) {
                        data = 1;
                    }
                    if (data < 0) {
                        data = 0;
                    }
                    file << static_cast<int>(data * 255) << " ";
                }
                file << "\n";
            }
        }
    }

    void setup_scene() {
        auto& material_light = m_scene.create<MaterialLambertianLight>(Vec3f {8, 8, 10});
        auto& material_red = m_scene.create<MaterialLambertian>(Vec3f {1, 0.4, 0.4});
        auto& material_blue = m_scene.create<MaterialLambertian>(Vec3f {0.4, 0.4, 1});
        auto& material_wall = m_scene.create<MaterialLambertian>(Vec3f {0.8, 0.8, 0.8});

        auto& light_sphere = m_scene.create<HittableSphere>(Vec3f {5, 0, 0}, 1, material_light);
        auto& small_sphere1 = m_scene.create<HittableSphere>(Vec3f {5, 2.2, 2.2}, 0.8, material_red);
        auto& small_sphere2 = m_scene.create<HittableSphere>(Vec3f {5, -2.2, -2.2}, 0.8, material_blue);
        auto& walls = build_cube(m_scene, Matrix4f::scale_matrix(10, 7, 7), material_wall);

        m_scene.get_root_hittable().add_children(small_sphere1);
        m_scene.get_root_hittable().add_children(small_sphere2);
        m_scene.get_root_hittable().add_children(light_sphere);
        m_scene.get_root_hittable().add_children(walls);

        m_renderer.render(m_scene_buffer, m_scene);
    }

    VK::UnownedInstance get_vk_instance() const {
        return m_vk_instance.unowned_copy();
    }

    const Camera& get_camera() const {
        return m_camera;
    }

    const SceneBuffer& get_scene_buffer() const {
        return m_scene_buffer;
    }
};