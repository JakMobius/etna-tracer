#pragma once

#include "../utils/vec3.hpp"

class Camera {

    Vec3f m_position {0, 0, 0};
    float m_camera_width = 1;
    float m_camera_height = 1;
    float m_focus_distance = 1;

    bool m_should_update_vectors = true;
    Vec3f m_calculated_width_vector {1, 0, 0};
    Vec3f m_calculated_height_vector {0, 1, 0};
    Vec3f m_calculated_focus_vector {0, 0, 1};

    void recalculate_vectors();

public:

    Camera() = default;
    ~Camera() = default;

    const Vec3f &get_position() const { return m_position; }
    float get_camera_width() const { return m_camera_width; }
    float get_camera_height() const { return m_camera_height; }
    float get_focus_distance() const { return m_focus_distance; }

    void set_position(const Vec3f& p_position) { m_position = p_position; m_should_update_vectors = true; }
    void set_camera_width(float camera_width) { camera_width = camera_width; m_should_update_vectors = true; }
    void set_camera_height(float camera_height) { camera_height = camera_height; m_should_update_vectors = true; }
    void set_focus_distance(float focus_distance) { focus_distance = focus_distance; m_should_update_vectors = true; }

    const Vec3f& get_calculated_width_vector() {
        if(m_should_update_vectors) recalculate_vectors();
        return m_calculated_width_vector;
    }

    const Vec3f& get_calculated_height_vector() {
        if(m_should_update_vectors) recalculate_vectors();
        return m_calculated_height_vector;
    }

    const Vec3f& get_calculated_focus_vector() {
        if(m_should_update_vectors) recalculate_vectors();
        return m_calculated_focus_vector;
    }

    bool is_moved() { return m_should_update_vectors; }
    void set_moved() { m_should_update_vectors = true; }

    Matrix4f matrix = {};
};