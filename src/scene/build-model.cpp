//
// Created by Артем on 06.10.2021.
//

#define TINYOBJLOADER_IMPLEMENTATION
#include "build-model.hpp"
#include "hittables/hittable-triangle.hpp"

Hittable& build_model(Scene& scene, const char* path, const Matrix4f& transform, Material& material) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::string err;

    auto& result = scene.create<HittableList>();

    if(!tinyobj::LoadObj(&attrib, &shapes, nullptr, &err, path, nullptr)) {
        std::cout << "Failed to load .obj file: " << err << "\n";
        return result;
    }

    std::cout << "Loading .obj file: " << path << " with vertex count = " << attrib.vertices.size() << "\n";

    for(auto& shape : shapes) {
        for(int i = 0; i < (int)shape.mesh.indices.size(); i += 3) {
            int index_a = shape.mesh.indices[i].vertex_index;
            int index_b = shape.mesh.indices[i + 1].vertex_index;
            int index_c = shape.mesh.indices[i + 2].vertex_index;

            Vec3f point_a { attrib.vertices[index_a * 3 + 0], attrib.vertices[index_a * 3 + 1], attrib.vertices[index_a * 3 + 2] };
            Vec3f point_b { attrib.vertices[index_b * 3 + 0], attrib.vertices[index_b * 3 + 1], attrib.vertices[index_b * 3 + 2] };
            Vec3f point_c { attrib.vertices[index_c * 3 + 0], attrib.vertices[index_c * 3 + 1], attrib.vertices[index_c * 3 + 2] };

            point_a *= transform;
            point_b *= transform;
            point_c *= transform;

            index_a = shape.mesh.indices[i].normal_index;
            index_b = shape.mesh.indices[i + 1].normal_index;
            index_c = shape.mesh.indices[i + 2].normal_index;

            Vec3f normal_a { attrib.normals[index_a * 3 + 0], attrib.normals[index_a * 3 + 1], attrib.normals[index_a * 3 + 2] };
            Vec3f normal_b { attrib.normals[index_b * 3 + 0], attrib.normals[index_b * 3 + 1], attrib.normals[index_b * 3 + 2] };
            Vec3f normal_c { attrib.normals[index_c * 3 + 0], attrib.normals[index_c * 3 + 1], attrib.normals[index_c * 3 + 2] };

            normal_a.transform_unbound(transform);
            normal_b.transform_unbound(transform);
            normal_c.transform_unbound(transform);

            auto& triangle = scene.create<HittableTriangle>(point_a, point_b, point_c, material);
            triangle.set_normals(normal_a.normal(), normal_b.normal(), normal_c.normal());

            result.add_children(triangle);
        }
    }

    return result;
}