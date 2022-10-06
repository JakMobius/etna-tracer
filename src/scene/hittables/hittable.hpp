#pragma once

class Hittable;

#include <queue>
#include "../scene.hpp"
#include "../scene-buffer-serializable.hpp"
#include "../scene-renderer.hpp"
#include "../scene-resource.hpp"
#include "../aabb.hpp"

class Hittable : public SceneBufferSerializable, public SceneResource {
protected:
    AABB m_bounding_box;
    bool m_bounding_box_valid = false;

public:
    Hittable(): m_bounding_box() {};
    virtual ~Hittable() {};

    virtual void register_hittables(SceneRenderer& renderer);
    virtual void register_materials(SceneRenderer& renderer);
    virtual void flatten(std::vector<Hittable*>* storage) { storage->push_back(this); };
    virtual void update_aabb() = 0;

    AABB get_bounding_box() {
        if(!m_bounding_box_valid) {
            update_aabb();
            m_bounding_box_valid = true;
        }
        return m_bounding_box;
    }
};