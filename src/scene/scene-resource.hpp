#pragma once

class SceneResource {
    SceneResource(const SceneResource&) = delete;
    SceneResource& operator=(const SceneResource&) = delete;
public:
    SceneResource() = default;
    SceneResource(SceneResource&&) = default;
    virtual ~SceneResource() = default;
};