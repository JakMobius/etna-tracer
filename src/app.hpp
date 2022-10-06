
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

class App {
    VK::Instance m_vk_instance;
    Scene m_scene;
    Camera m_camera;
    SceneRenderer m_renderer;
    SceneBuffer m_scene_buffer;
    std::vector<std::unique_ptr<Renderer>> m_renderers;
    float m_gamma = 0.36;
    int m_render_time = 3;
    int m_screen_width = 800;
    int m_screen_height = 800;

    std::atomic<bool> m_stop{false};
    std::vector<std::thread> m_threads;

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

    void write_output_to_file();

    void setup_scene() {
        m_camera.set_position({0, 0, -1.2});
        m_camera.matrix = m_camera.matrix * Matrix4f::rotation_y_matrix(0.18);
        m_camera.set_moved();

//        m_scene.get_root_hittable().add_child(disco_spheres_scene(m_scene));
        m_scene.get_root_hittable().add_child(tank_scene(m_scene));
        m_renderer.render(m_scene_buffer, m_scene);
    }

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