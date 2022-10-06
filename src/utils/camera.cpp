
#include "camera.hpp"

void Camera::recalculate_vectors() {
    m_should_update_vectors = false;

    m_calculated_focus_vector = Vec3f {m_focus_distance, 0, 0};
    m_calculated_width_vector = Vec3f {0, m_camera_width, 0};
    m_calculated_height_vector = Vec3f {0, 0, m_camera_height};

    m_calculated_focus_vector *= matrix;
    m_calculated_width_vector *= matrix;
    m_calculated_height_vector *= matrix;
}