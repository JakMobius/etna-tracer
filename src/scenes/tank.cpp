
#include <random>
#include "tank.hpp"
#include "../scene/hittables/hittable-sphere.hpp"
#include "../scene/hittables/hittable-list.hpp"
#include "../scene/materials/material-lambertian.hpp"
#include "../scene/materials/material-lambertian-light.hpp"
#include "../scene/build-rect.hpp"
#include "../scene/hittables/hittable-triangle.hpp"
#include "../scene/build-model.hpp"
#include "../scene/materials/material-dielectric.hpp"

Hittable &tank_scene(Scene &scene) {
    auto &result = scene.create<HittableList>();

    auto &material_light = scene.create<MaterialLambertianLight>(Vec3f{8, 8, 10});
    auto &tank_glass = scene.create<MaterialDielectric>(Vec3f{1, 1, 1}, 1.5);
    auto &tank_material = scene.create<MaterialLambertian>(Vec3f{0.5, 1.0, 0.5});
    auto &material_wall = scene.create<MaterialLambertian>(Vec3f{0.8, 0.8, 0.8});

//    auto &floor = scene.create<HittableTriangle>(Vec3f{5, 5, 2}, Vec3f{5, -5, 2}, Vec3f{-5, 0, 2}, material_wall);

    auto tank_matrix = Matrix4f::rotation_x_matrix(-M_PI / 2) *
                       Matrix4f::rotation_z_matrix(0.6) *
                       Matrix4f::scale_matrix(0.1, 0.1, 0.1) *
                       Matrix4f::translation_matrix(7, -1, -0.6);

    auto &tank_outer_model = build_model(scene, "../models/tank-expanded.obj", tank_matrix, tank_glass);
    auto &tank_model = build_model(scene, "../models/tank.obj", tank_matrix, tank_material);

    for (int i = -5; i <= 5; i++) {
        for (int j = -5; j <= 5; j++) {
            float x = static_cast<float>(i) * 1.5f;
            float y = static_cast<float>(j) * 1.5f;

//            auto &light_sphere = scene.create<HittableSphere>(Vec3f{x, y, -10}, 0.5, material_light);
//            result.add_child(light_sphere);
        }
    }

    result.add_child(tank_outer_model);
    result.add_child(tank_model);

    // Generate some stars

    float star_distance = 5000;
    float star_size = 6;

    std::mt19937 generator(1);
    std::normal_distribution<float> normal_distribution(0, 1);
    std::uniform_real_distribution<float> uniform_distribution(0, 1);

    for(int i = 0; i < 400; i++) {
        Vec3f random_unit_vector{};

        do {
            random_unit_vector = Vec3f{normal_distribution(generator),
                                       normal_distribution(generator),
                                       normal_distribution(generator)};
        } while (random_unit_vector.len() > 1);

        random_unit_vector.normalize();

        random_unit_vector.set(0, std::abs(random_unit_vector[0]));
        random_unit_vector.set(2, -std::abs(random_unit_vector[2]));

        Vec3f position = random_unit_vector * star_distance;
        float radius = star_size;

        Vec3f color {
            0.95f + uniform_distribution(generator) * 0.05f,
            0.95f + uniform_distribution(generator) * 0.05f,
            0.95f + uniform_distribution(generator) * 0.05f
        };

//        auto &material = scene.create<MaterialLambertianLight>(color);
//        auto &sphere = scene.create<HittableSphere>(position, radius, material);
//        result.add_child(sphere);
    }

    return result;
}