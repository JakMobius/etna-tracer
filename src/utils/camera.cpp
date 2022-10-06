
#include "camera.hpp"

void Camera::recalculate_vectors() {
    should_update_vectors = false;

    calculated_focus_vector = Vec3f {focus_distance, 0, 0};
    calculated_width_vector = Vec3f {0, camera_width, 0};
    calculated_height_vector = Vec3f {0, 0, camera_height};

    calculated_focus_vector *= matrix;
    calculated_width_vector *= matrix;
    calculated_height_vector *= matrix;
}