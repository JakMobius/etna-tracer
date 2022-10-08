
#include <etna/init.hpp>
#include <etna/vk-wrappers/device/vk-physical-device.hpp>
#include <iostream>
#include <thread>
#include <etna/vk-wrappers/instance/vk-instance-factory.hpp>
#include <etna/vk-wrappers/vk-debug-callback-handler.hpp>
#include "renderer.hpp"
#include "scene/materials/material-dielectric.hpp"
#include "utils/vec2.hpp"
#include "scenes/disco-spheres.hpp"
#include "scenes/tank.hpp"
#include "scene/materials/material-atmosphere.hpp"

class App {
    VK::Instance m_vk_instance;
    Scene m_scene;
    Camera m_camera;
    SceneRenderer m_renderer;
    SceneBuffer m_scene_buffer;
    std::vector<std::unique_ptr<Renderer>> m_renderers;
    float m_gamma = 0.36;
    int m_render_time = 3;
    int m_screen_width = 1000;
    int m_screen_height = 1000;

    int m_atmosphere_sphere_index = -1;
    int m_atmosphere_material_index = -1;
    std::atomic<bool> m_stop{false};
    std::vector<std::thread> m_threads;

    HittableSphere* m_sun_sphere = nullptr;
    MaterialAtmosphere* m_atmosphere = nullptr;
    HittableSphere* m_atmosphere_sphere = nullptr;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_render_start_time;

    bool m_camera_in = true;
    bool m_rendering = false;

public:
    int main();

    void render();

    void wait_for_threads();

    int get_screen_width() const {
        return m_screen_width;
    }

    int get_screen_height() const {
        return m_screen_height;
    }

    void write_output_to_file(const std::string& file_name);

    void setup_scene();

    void tick_scene(float t);

    VK::UnownedInstance get_vk_instance() const {
        return m_vk_instance.unowned_copy();
    }

    const Camera &get_camera() const {
        return m_camera;
    }

    const SceneBuffer &get_scene_buffer() const {
        return m_scene_buffer;
    }
};