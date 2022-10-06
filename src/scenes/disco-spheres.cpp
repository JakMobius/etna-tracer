
#include <random>
#include "disco-spheres.hpp"
#include "../scene/hittables/hittable-sphere.hpp"
#include "../scene/hittables/hittable-list.hpp"
#include "../scene/materials/material-lambertian.hpp"
#include "../scene/materials/material-lambertian-light.hpp"
#include "../scene/build-cube.hpp"
#include "../scene/materials/material-dielectric.hpp"

Hittable &disco_spheres_scene(Scene &scene) {
    auto& result = scene.create<HittableList>();

    Vec3f lamp_position {5, 0, 0 };

    auto &material_light = scene.create<MaterialLambertianLight>(Vec3f{8, 8, 10});
    auto &material_red = scene.create<MaterialLambertian>(Vec3f{1, 0.4, 0.4});
    auto &material_blue = scene.create<MaterialLambertian>(Vec3f{0.4, 0.4, 1});
    auto &material_wall = scene.create<MaterialLambertian>(Vec3f{0.8, 0.8, 0.8});

    auto &light_sphere = scene.create<HittableSphere>(lamp_position, 1, material_light);
    auto &small_sphere1 = scene.create<HittableSphere>(Vec3f{5, 2.2, 2.2}, 0.8, material_red);
    auto &small_sphere2 = scene.create<HittableSphere>(Vec3f{5, -2.2, -2.2}, 0.8, material_blue);
    auto &walls = build_cube(scene, Matrix4f::scale_matrix(10, 7, 7), material_wall);

    // Generate some random spheres

    std::vector<HittableSphere *> mini_spheres;

    std::mt19937 generator(1);
    std::normal_distribution<float> normal_distribution(0, 1);
    std::uniform_real_distribution<float> uniform_distribution(0, 1);

    while(true) {
        bool could_place = false;
        for(int j = 0; j < 1000; j++) {

            Vec3f random_unit_vector{};

            do {
                random_unit_vector = Vec3f{normal_distribution(generator),
                                           normal_distribution(generator),
                                           normal_distribution(generator)};
            } while (random_unit_vector.len() > 1);

            random_unit_vector.normalize();

            Vec3f position = lamp_position + random_unit_vector * 1.5;
            float radius = 0.1;

            could_place = !std::any_of(mini_spheres.begin(), mini_spheres.end(), [&](HittableSphere *sphere) {
                float dist = (sphere->get_position() - position).len();
                float min_dist = sphere->get_radius() + radius;

                return dist < min_dist;
            });

            if (!could_place) continue;

            Vec3f color {
                    uniform_distribution(generator),
                    uniform_distribution(generator),
                    uniform_distribution(generator)
            };

            auto &material = scene.create<MaterialDielectric>(color, 1.5, 0.05, 0.05);
            auto &sphere = scene.create<HittableSphere>(position, radius, material);
            mini_spheres.push_back(&sphere);
            result.add_child(sphere);

            break;
        }
        if(!could_place) break;
    }

    result.add_child(small_sphere1);
    result.add_child(small_sphere2);
    result.add_child(light_sphere);
    result.add_child(walls);

    return result;
}
